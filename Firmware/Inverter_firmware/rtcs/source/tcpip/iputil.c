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
* $FileName: iputil.c$
* $Version : 3.8.9.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains various IP layer functions.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"

#if RTCSCFG_ENABLE_IP4 

/* Used for most of the IP utility functions */
struct IP_util_struct {
   IP_IF_PTR      ipif;
   _ip_address    ipaddr;
   boolean        bool;
   uint_32        num;
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : IP_get_netmask
* Returned Value   : boolean
* Comments         : Gets the network mask. Returns TRUE if found, or
*                    FALSE if no netmask is found.
*
*END*-----------------------------------------------------------------*/

struct IPIF_getmask_struct {
   IP_IF_PTR      netif;
   _ip_address    address;
   _ip_address    mask;
};

static boolean IP_get_netmask_test
   (
      pointer  node,
      pointer  data
   )
{ /* Body */
   IP_ROUTE_PTR                      route = node;
   struct IPIF_getmask_struct _PTR_  getmask = data;
   IP_ROUTE_DIRECT_PTR               search_ptr;

   if (route->DIRECT) {
      search_ptr = route->DIRECT;
      do {
         if (search_ptr->ADDRESS == getmask->address &&
             search_ptr->DESTIF == getmask->netif)
         {
            getmask->mask = route->NODE.MASK;
            return TRUE;
         } /* Endif */
         search_ptr = search_ptr->NEXT;
      } while(search_ptr != route->DIRECT);
   } /* Endif */

   return FALSE;

} /* Endbody */


boolean IP_get_netmask
   (
      _rtcs_if_handle   ihandle,    /* [IN] Interface */
      _ip_address       address,    /* [IN] IP address */
      _ip_address _PTR_ mask_ptr    /* [OUT] netwask for the IP and interface */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg( IP );
   struct IPIF_getmask_struct    getmask;

   getmask.netif     = ihandle;
   getmask.address   = address;
   getmask.mask      = 1;   /* Invalid mask */

   IPRADIX_findbest(&IP_cfg_ptr->ROUTE_ROOT.NODE, address,
      IP_get_netmask_test, &getmask);

   if (getmask.mask == 1) {
      return FALSE;
   } /* Endif */

   *mask_ptr = getmask.mask;
   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_get_ipif_addr
*  Returned Value : the ip address
*  Comments       : return the ip address of the interface
*
*END*-----------------------------------------------------------------*/

static boolean IP_get_ipif_addr_test
   (
      _ip_address    node_ip,
      _ip_address    node_mask,
      pointer        node_data,
      pointer        data
   )
{ /* Body */
   struct IP_util_struct _PTR_   testdata = data;
   IP_ROUTE_PTR                  route = node_data;
   IP_ROUTE_DIRECT_PTR           search_ptr;

   /* Make sure there are direct interfaces, with full IP addresses */
   if (!~node_mask && route && route->DIRECT) {
      search_ptr = route->DIRECT;
      do {
         if (search_ptr->NETIF == testdata->ipif) {
            testdata->ipaddr = node_ip;
            return TRUE;
         } /* Endif */
         search_ptr = search_ptr->NEXT;
      } while(search_ptr != route->DIRECT);
   } /* Endif */

   return FALSE;

} /* Endbody */


_ip_address IP_get_ipif_addr
   (
      IP_IF_PTR      ipif        /* [IN] the local interface */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg( IP );
   struct IP_util_struct   testdata;

   testdata.ipif     = ipif;
   testdata.ipaddr   = INADDR_ANY;

   /* Check all nodes of the tree */
   IPRADIX_walk(&IP_cfg_ptr->ROUTE_ROOT.NODE, IP_get_ipif_addr_test, &testdata);

   return testdata.ipaddr;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_is_local
*  Returned Value : TRUE or FALSE
*  Comments       :
*        Decides whether or not an IP address is local to an interface.
*        if iflocal is null, check all the interfaces.
*
*END*-----------------------------------------------------------------*/

static void IP_is_local_test
   (
      pointer _PTR_  node,
      pointer        data
   )
{ /* Body */
   struct IP_util_struct _PTR_   testdata = data;
   IP_ROUTE_PTR                  route = NULL;
   IP_ROUTE_DIRECT_PTR           search_ptr = NULL;

   if (*node) {
      route = *node;
      search_ptr = route->DIRECT;
   } /* Endif */

   if (search_ptr) {
      do {
         if ((search_ptr->NETIF == testdata->ipif || testdata->ipif == NULL) &&
             (testdata->ipaddr == search_ptr->ADDRESS))
         {
            testdata->bool = TRUE;
            break;
         } /* Endif */
         search_ptr = search_ptr->NEXT;
      } while(search_ptr != route->DIRECT);
   } /* Endif */

} /* Endbody */


boolean IP_is_local
   (
      IP_IF_PTR      iflocal,    /* [IN] the local interface */
      _ip_address    iplocal     /* [IN] the IP address to test */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg(IP);
   struct IP_util_struct   testdata;

   testdata.ipif     = iflocal;
   testdata.ipaddr   = iplocal;
   testdata.bool     = FALSE;

   /* Will make testdata.bool TRUE if ip is local to interface */
   IPRADIX_insert(&IP_cfg_ptr->ROUTE_ROOT.NODE, iplocal, 0xFFFFFFFF, 0,
      IP_is_local_test, &testdata);

   return testdata.bool;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_is_direct
*  Returned Value : TRUE or FALSE
*  Comments       :
*        Decides whether or not an IP address can be directly reached
*        through a given local interface.
*
*END*-----------------------------------------------------------------*/

static boolean IP_is_direct_test
   (
      pointer        node_data,
      pointer        data
   )
{ /* Body */
   struct IP_util_struct _PTR_   testdata = data;
   IP_ROUTE_PTR                  route = node_data;

   if (route->DIRECT) {
      IP_ROUTE_DIRECT_PTR direct = route->DIRECT;
      do {
         if ((direct->DESTIF == testdata->ipif || testdata->ipif == NULL)) {
            testdata->bool = TRUE;
            return TRUE;
         } /* Endif */
         direct = direct->NEXT;
      } while(direct != route->DIRECT);
   } /* Endif */

   if (route->VIRTUAL) {
      IP_ROUTE_VIRTUAL_PTR virtual = route->VIRTUAL;
      do {
         if ((virtual->DESTIF == testdata->ipif || testdata->ipif == NULL)) {
            testdata->bool = TRUE;
            return TRUE;
         } /* Endif */
         virtual = virtual->NEXT;
      } while(virtual != route->VIRTUAL);
   } /* Endif */

   return FALSE;
} /* Endbody */


boolean IP_is_direct
   (
      IP_IF_PTR      iflocal,
       /* [IN] the local interface */
      _ip_address    iplocal
       /* [IN] the IP address to test */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg(IP);
   struct IP_util_struct   testdata;

   testdata.ipif     = iflocal;
   testdata.ipaddr   = iplocal;
   testdata.bool     = FALSE;

   IPRADIX_findbest(&IP_cfg_ptr->ROUTE_ROOT.NODE, iplocal,
      IP_is_direct_test, &testdata);

   return testdata.bool;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_is_gate
*  Returned Value : TRUE or FALSE
*  Comments       :
*        Decides whether or not an IP address is a gateway for another.
*
*END*-----------------------------------------------------------------*/

static boolean IP_is_gate_test
   (
      pointer        node_data,
      pointer        data
   )
{ /* Body */
   struct IP_util_struct _PTR_   testdata = data;
   IP_ROUTE_PTR                  route = node_data;
   IP_ROUTE_INDIRECT_PTR         indirect;

   if (route->INDIRECT) {
      indirect = route->INDIRECT;
      do {
         if (indirect->GATEWAY == testdata->ipaddr) {
            testdata->bool = TRUE;
            return TRUE;
         } /* Endif */
         indirect = indirect->NEXT;
      } while(indirect != route->INDIRECT);
   } /* Endif */

   return FALSE;

} /* Endbody */


boolean IP_is_gate
   (
      _ip_address    gateway,
            /* [IN] the gateway */
      _ip_address    ipremote
            /* [IN] the IP address to test */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg(IP);
   struct IP_util_struct   data;

   data.ipaddr = gateway;
   data.bool = FALSE;

   IPRADIX_findbest(&IP_cfg_ptr->ROUTE_ROOT.NODE, ipremote,
      IP_is_gate_test, &data);

   return data.bool;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_MTU
*  Returned Value : MTU for destination
*  Comments       :
*        Determines the Maximum Transmission Unit for a destination;
*        returns IP_DEFAULT_MTU if destination is not directly connected.
*
*END*-----------------------------------------------------------------*/

static boolean IP_MTU_test
   (
      pointer        node_data,
      pointer        data
   )
{ /* Body */
   struct IP_util_struct _PTR_   testdata = data;
   IP_ROUTE_PTR                  route = node_data;
   IP_ROUTE_VIRTUAL_PTR          virtual;

   if (route->DIRECT) {
      testdata->num = route->DIRECT->DESTIF->MTU;
      return TRUE;
   } /* Endif */

   virtual = route->VIRTUAL;
   if (virtual) {
      if ((testdata->ipaddr & virtual->SOURCE_MASK) == virtual->SOURCE_NET) {
         if (virtual->DESTIF->MTU_FN) {
            testdata->num = virtual->DESTIF->MTU_FN(virtual->DATA);
         } else {
            testdata->num = virtual->DESTIF->MTU;
         } /* Endif */
         return TRUE;
      } /* Endif */
   } /* Endif */

   return FALSE;

} /* Endbody */


uint_32 IP_MTU
   (
      _ip_address    iplocal,
            /* [IN] the local IP address */
      _ip_address    ipremote
            /* [IN] the remote IP address to test */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg(IP);
   struct IP_util_struct   data;

   data.ipaddr = iplocal;
   data.num    = IP_DEFAULT_MTU;

   IPRADIX_findbest(&IP_cfg_ptr->ROUTE_ROOT.NODE, ipremote,
      IP_MTU_test, &data);

   return data.num;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_find_if
*  Returned Value : interface handle
*  Comments       :
*        Returns the interface handle for a local IP address.
*
*END*-----------------------------------------------------------------*/

static void IP_find_if_test
   (
      pointer _PTR_  node,
      pointer        data
   )
{ /* Body */
   struct IP_util_struct _PTR_   testdata = data;
   IP_ROUTE_PTR                  route;

   if (*node) {
      route = *node;
      if (route->DIRECT) {
         testdata->ipif = route->DIRECT->NETIF;
      } /* Endif */
   } /* Endif */

} /* Endbody */


IP_IF_PTR IP_find_if
   (
      _ip_address    iplocal
            /* [IN] the IP address to test */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg(IP);
   struct IP_util_struct   data;

   data.ipif     = NULL;
   data.ipaddr   = iplocal;

   IPRADIX_insert(&IP_cfg_ptr->ROUTE_ROOT.NODE, iplocal, 0xFFFFFFFF, 0,
      IP_find_if_test, &data);

   return data.ipif;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_find
*  Returned Value : source IP address
*  Comments       :
*        Determine the source IP address to use when routing to a
*        specific destination.
*
*END*-----------------------------------------------------------------*/

static boolean IP_route_find_test
   (
      pointer  node,
      pointer  data
   )
{ /* Body */
   struct IP_util_struct _PTR_   testdata = data;
   IP_ROUTE_PTR                  route = node;

   /* Check for a direct route. */
   if (route->DIRECT) {
      testdata->ipaddr = route->DIRECT->ADDRESS;
      return TRUE;
   } /* Endif */

   /* Check for a numbered virtual route. */
   if (route->VIRTUAL) {
      IP_ROUTE_VIRTUAL_PTR virtual = route->VIRTUAL;
      do {
         if (virtual->ADDRESS) {
            testdata->ipaddr = virtual->ADDRESS;
            return TRUE;
         } /* Endif */
         virtual = virtual->NEXT;
      } while (virtual != route->VIRTUAL);
   } /* Endif */

   /*
   ** If no direct route was found, search for a gateway.
   */
   if (testdata->bool && route->INDIRECT) {
      IP_ROUTE_INDIRECT_PTR indirect = route->INDIRECT;
      do {
         if (indirect->FLAGS & RTF_UP) {
            testdata->ipaddr = indirect->GATEWAY;
            testdata->bool = FALSE;
            return TRUE;
         } /* Endif */
         indirect = indirect->NEXT;
      } while (indirect != route->INDIRECT);
   } /* Endif */

   return FALSE;
} /* Endbody */


_ip_address IP_route_find
   (
      _ip_address    ipdest,
            /* [IN] the ultimate destination */
      uint_32        flags
            /* [IN] optional flags */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg(IP);
   struct IP_util_struct   data;
   _ip_address             gateway;

   data.ipaddr  = INADDR_ANY;
   if (!flags) {
      data.bool   = TRUE;       /* Check gateways */
   } else {
      data.bool   = FALSE;
   } /* Endif */

   IPRADIX_findbest(&IP_cfg_ptr->ROUTE_ROOT.NODE, ipdest,
      IP_route_find_test, &data);

   /* If we have found a gateway, find source addr for the gateway */
   if (!data.bool) {
      gateway = data.ipaddr;
      data.ipaddr = INADDR_ANY;
      IPRADIX_findbest(&IP_cfg_ptr->ROUTE_ROOT.NODE, gateway,
         IP_route_find_test, &data);
   } /* Endif */

   return data.ipaddr;
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCS_get_enet_handle
*  Returned Value : ethernet handle (pointer)
*  Comments       :
*        Returns the ethernet handle associated with given interface.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_get_enet_handle
   (
      _rtcs_if_handle   ihandle    /* [IN] Interface */
   )
{
   return ((IP_IF_PTR)ihandle)->HANDLE;
}

#if RTCSCFG_ENABLE_IP6

/************************************************************************
* NAME: IP6_MTU
* RETURNS : MTU for destination
* DESCRIPTION: Determines the Maximum Transmission Unit for a destination;
*        returns IP6_DEFAULT_MTU if destination is not directly connected
*        or source is not our address.
*************************************************************************/
uint_32 IP6_MTU ( in6_addr *iplocal     /* [IN] the local IP address */,
                  in6_addr *ipremote    /* [IN] the remote IP address to test */ )
{ 
    uint_32 mtu = IP_DEFAULT_MTU;
    IP_IF_PTR dest_if; 

    if( iplocal == NULL)
    /* Determine a source address.*/
    {
        iplocal = (in6_addr *)ip6_if_select_src_addr(NULL, ipremote);
                
        if(iplocal == NULL)
        {
            goto FAIL;;
        } 
    }

    dest_if = ip6_if_get_by_addr(iplocal);
        
    if(dest_if)
    {
        mtu = dest_if->IP6_IF.ND6->mtu;
    }
    
FAIL:    
    return mtu;
}

/************************************************************************
* NAME: IP6_route_find
* RETURNS : Source IPv6 address.
* DESCRIPTION: Determine the source IPv6 address to use when routing to a
*              specific destination.
*************************************************************************/
in6_addr * IP6_route_find( in6_addr *ipdest   /* [IN] the ultimate destination */,
                                 uint_32 flags      /* [IN] optional flags (Not used - TBD delete it*/)
{
    in6_addr * addr = (in6_addr *)ip6_if_select_src_addr(NULL, ipdest); 

    return addr;
}

#endif /* RTCSCFG_ENABLE_IP6 */


/* EOF */
