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
* $FileName: ipif_rem.c$
* $Version : 3.8.6.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the
*   packet driver interface.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_remove
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     not used
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     not used
*
*  Comments       :
*        Unregisters a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/

void IPIF_remove
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr;
   IP_IF_PTR         ipif;
   uint_32           error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   ipif = (IP_IF_PTR)parms->ihandle;
   error = ipif->DEVICE.CLOSE(ipif);

   /* Enqueue onto the free list */
   ipif->HANDLE = IF_FREE;
   IF_FREE = ipif;

   RTCSCMD_complete(parms, error);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_unbind
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the local address
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     not used
*
*  Comments       :
*        Unbinds an IP address and network from a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_unbind
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   _ip_address    mask;

   IP_route_remove_direct(parms->address, 0xFFFFFFFF, parms->ihandle);
   
#if RTCSCFG_ENABLE_IGMP 
   if (parms->ihandle) {
      ((IP_IF_PTR)(parms->ihandle))->IGMP_UNBIND((IP_IF_PTR)parms->ihandle,
         parms->address);
   } /* Endif */
#endif
   IP_get_netmask(parms->ihandle, parms->address, &mask);
   
   /* If mask is invalid do not try to remove direct route */
   if (mask != 1)
   {
        IP_route_remove_direct(parms->address, mask, parms->ihandle);
   }
   RTCSCMD_complete(parms, RTCS_OK);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */
   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_unbind_ppp
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the local address
*     _ip_address          locmask     not used
*     _ip_address          network     [IN] the peer address
*     _ip_address          netmask     not used
*
*  Comments       :
*        Unbinds an IP address and network from a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_unbind_ppp
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   IP_route_remove_virtual(parms->network, 0xFFFFFFFF, parms->address,
      INADDR_ANY, INADDR_ANY, parms->ihandle);
      
#if RTCSCFG_ENABLE_IGMP
   if (parms->ihandle) {
      ((IP_IF_PTR)(parms->ihandle))->IGMP_UNBIND((IP_IF_PTR)parms->ihandle,
         parms->address);
   } /* Endif */
#endif

   IP_route_remove_direct(parms->address, 0xFFFFFFFF, parms->ihandle);

   RTCSCMD_complete(parms, RTCS_OK);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */
   

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_gate_remove
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     not used
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the gateway address
*     _ip_address          locmask     not used
*     _ip_address          network     [IN] the network address
*     _ip_address          netmask     [IN] the network address mask
*
*  Comments       :
*        Removes a gateway from the routing table.
*
*END*-----------------------------------------------------------------*/

void IPIF_gate_remove
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

    /* Start CR 1133 */
    IP_route_remove_indirect(parms->address, parms->netmask, parms->network, RTF_STATIC, parms->locmask);
    /* End CR 1133 */

    RTCSCMD_complete(parms, RTCS_OK);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */
   

} /* Endbody */


/* EOF */
