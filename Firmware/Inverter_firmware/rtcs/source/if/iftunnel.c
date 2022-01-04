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
* $FileName: iftunnel.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the interface functions to the
*   RTCS tunnel device.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "iptunnel.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_tunnel_add
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Adds an IP over IP tunnel to RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_tunnel_add
   (
      /* [IN] Inner IP header source address to tunnel */
      _ip_address  inner_source_addr,
      /* [IN] Inner IP header source address network mask */
      _ip_address  inner_source_netmask,
      /* [IN] Inner IP header destination address to tunnel */
      _ip_address  inner_dest_addr,
      /* [IN] Inner IP header destination network mask */
      _ip_address  inner_dest_netmask,
      /* [IN] Outer IP header source address */
      _ip_address  outer_source_addr,
      /* [IN] Outer IP header destination address */
      _ip_address  outer_dest_addr,
      /* [IN] Flags to set tunnel behaviour */
      uint_32 flags
   )
{ /* Body */
   IPIP_PARM  parms;

   parms.TUNNEL.INNER_SOURCE         = inner_source_addr & inner_source_netmask;
   parms.TUNNEL.INNER_SOURCE_NETMASK = inner_source_netmask;
   parms.TUNNEL.INNER_DEST           = inner_dest_addr & inner_dest_netmask;
   parms.TUNNEL.INNER_DEST_NETMASK   = inner_dest_netmask;
   parms.TUNNEL.OUTER_DEST           = outer_dest_addr;
   parms.TUNNEL.OUTER_SOURCE         = outer_source_addr;
   parms.TUNNEL.FLAGS                = flags;

   return RTCSCMD_issue(parms, IPIP_insert);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_tunnel_remove
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Removes an IP over IP tunnel from RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_tunnel_remove
   (
      /* [IN] Inner IP header source address to tunnel */
      _ip_address  inner_source_addr,
      /* [IN] Inner IP header source address network mask */
      _ip_address  inner_source_netmask,
      /* [IN] Inner IP header destination address to tunnel */
      _ip_address  inner_dest_addr,
      /* [IN] Inner IP header destination network mask */
      _ip_address  inner_dest_netmask,
      /* [IN] Outer IP header source address */
      _ip_address  outer_source_addr,
      /* [IN] Outer IP header destination address */
      _ip_address  outer_dest_addr,
      /* [IN] Flags to set tunnel behaviour */
      uint_32 flags
   )
{ /* Body */
   IPIP_PARM  parms;

   parms.TUNNEL.INNER_SOURCE         = inner_source_addr & inner_source_netmask;
   parms.TUNNEL.INNER_SOURCE_NETMASK = inner_source_netmask;
   parms.TUNNEL.INNER_DEST           = inner_dest_addr & inner_dest_netmask;
   parms.TUNNEL.INNER_DEST_NETMASK   = inner_dest_netmask;
   parms.TUNNEL.OUTER_DEST           = outer_dest_addr;
   parms.TUNNEL.OUTER_SOURCE         = outer_source_addr;
   parms.TUNNEL.FLAGS                = flags;

   return RTCSCMD_issue(parms, IPIP_delete);

} /* Endbody */


/* EOF */
