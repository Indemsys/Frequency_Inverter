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
* $FileName: ifadd.c$
* $Version : 3.8.9.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the interface functions to the
*   packet driver interface.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_add
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_add
   (
      pointer                 mhandle,
         /* [IN] the packet driver handle */
      RTCS_IF_STRUCT_PTR      if_ptr,
         /* [IN] call table for the interface */
      _rtcs_if_handle _PTR_   handle
         /* [OUT] the RTCS interface state structure */
   )
{ /* Body */
   IPIF_PARM   parms;
   uint_32     error;

   if (mhandle==NULL) 
   {
      return RTCSERR_INVALID_PARAMETER; 
   }
   
   parms.mhandle = mhandle;
   parms.if_ptr  = if_ptr;

   error = RTCSCMD_issue(parms, IPIF_add);

   if (!error) {
      *handle = parms.ihandle;
   } /* Endif */
   return error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_bind
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Bind an IP address to a registered hardware interface.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_bind
   (
      _rtcs_if_handle   handle,
         /* [IN] the RTCS interface state structure */
      _ip_address       address,
         /* [IN] the IP address for the interface */
      _ip_address       netmask
         /* [IN] the IP (sub)network mask for the interface */
   )
{ /* Body */
   IPIF_PARM   parms;

   if (handle==NULL) 
   {
      return RTCSERR_INVALID_PARAMETER; 
   }
   
   parms.ihandle = handle;
   parms.address = address;
   parms.locmask = 0xFFFFFFFFL;
   parms.netmask = netmask;
   parms.probe   = FALSE;

   return RTCSCMD_issue(parms, IPIF_bind);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_probe_and_bind
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Bind an IP address to a registered hardware interface.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_probe_and_bind
   (
      _rtcs_if_handle   handle,
         /* [IN] the RTCS interface state structure */
      _ip_address       address,
         /* [IN] the IP address for the interface */
      _ip_address       netmask
         /* [IN] the IP (sub)network mask for the interface */
   )
{ /* Body */
   IPIF_PARM   parms;
   uint_32     error;

   if (handle==NULL) 
   {
      return RTCSERR_INVALID_PARAMETER; 
   }

   parms.ihandle = handle;
   parms.address = address;
   parms.locmask = 0xFFFFFFFFL;
   parms.netmask = netmask;
   parms.probe   = TRUE;
   error = RTCSCMD_issue(parms, IPIF_bind);
   
   if (RTCS_OK == error)
   {
       _time_delay (500);
       
       parms.ihandle = handle;
       parms.address = address;
       parms.locmask = 0xFFFFFFFFL;
       parms.netmask = netmask;
       parms.probe   = TRUE;
       error = RTCSCMD_issue(parms, IPIF_bind_finish);
   }

   return error;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gate_add
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a gateway with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_gate_add
   (
      _ip_address       gateway,
         /* [IN] the IP address of the gateway */
      _ip_address       network,
         /* [IN] the IP (sub)network to route */
      _ip_address       netmask
         /* [IN] the IP (sub)network mask to route */
   )
{ /* Body */
#if RTCSCFG_ENABLE_GATEWAYS
   IPIF_PARM   parms;

   parms.address = gateway;
   parms.network = network;
   parms.netmask = netmask;
   /* Start CR 1133 */
   parms.locmask = 0;
   /* End CR 1133 */

   return RTCSCMD_issue(parms, IPIF_gate_add);
#else
   return RTCSERR_FEATURE_NOT_ENABLED;
#endif
} /* Endbody */


/* Start CR 1133 */
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gate_add_metric
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a gateway with RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_gate_add_metric
   (
      _ip_address       gateway,
         /* [IN] the IP address of the gateway */
      _ip_address       network,
         /* [IN] the IP (sub)network to route */
      _ip_address       netmask,
         /* [IN] the IP (sub)network mask to route */
      uint_16           metric
         /* [IN] the metric of the gateway */
   )
{ /* Body */
#if RTCSCFG_ENABLE_GATEWAYS
   IPIF_PARM   parms;

   parms.address = gateway;
   parms.network = network;
   parms.netmask = netmask;
   parms.locmask = metric;

   return RTCSCMD_issue(parms, IPIF_gate_add);
#else
   return RTCSERR_FEATURE_NOT_ENABLED;
#endif

} /* Endbody */
/* End CR 1133 */

#if RTCSCFG_ENABLE_IP6
/************************************************************************
* NAME: RTCS6_if_bind_addr
* RETURNS : RTCS_OK or error code
* DESCRIPTION: Bind an IPv6 address to a registered hardware interface.
*************************************************************************/
uint_32 RTCS6_if_bind_addr( _rtcs_if_handle   handle        /* [IN] the RTCS interface state structure */,
                            in6_addr  ip_addr               /* [IN] the IP address for the interface */,
                            ip6_if_addr_type ip_addr_type   /* Type.*/)
{
   IP6_IF_PARM   parms;

   if (handle==NULL) 
   {
      return RTCSERR_INVALID_PARAMETER; 
   }
   
   parms.ihandle = handle;
   parms.ip_addr = ip_addr;
   parms.ip_addr_type = ip_addr_type;
   
   return RTCSCMD_issue(parms, IP6_IF_bind_addr);
}
#endif /* RTCSCFG_ENABLE_IP6 */

/* EOF */
