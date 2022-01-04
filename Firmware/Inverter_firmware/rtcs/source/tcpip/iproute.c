/**HEADER********************************************************************
* 
* Copyright (c) 2008-2009 Freescale Semiconductor;
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
* $FileName: iproute.c$
* $Version : 3.8.14.0$
* $Date    : Sep-19-2012$
*
* Comments:
* 
*     This file contains the interface to the IP routing table.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP4  

#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"
#include "route.h"


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_init
*  Returned Value : void
*  Comments       :
*        Initializes the head of the routing table.
*
*END*-----------------------------------------------------------------*/

void IP_route_init
   (
      IP_ROUTE_PTR   head     /* Pointer to the head of the IPRADIX tree */
   )
{ /* Body */
   IPRADIX_init(&head->NODE);
   head->DIRECT = NULL;
   head->INDIRECT = NULL;
   head->VIRTUAL = NULL;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_insert
*  Returned Value : void
*  Comments       :
*        Adds a route to the routing table. Called by IPRADIX_insert.
*
*END*-----------------------------------------------------------------*/

struct ip_route_insert {
   uint_32           error;
   pointer           route;
   uint_32           index;
   void (_CODE_PTR_  sort)(pointer, pointer);
};

static void IP_route_insert
   (
      pointer _PTR_  node,
      pointer        data
   )
{ /* Body */
   struct ip_route_insert _PTR_     insertdata = data;
   IP_ROUTE_PRV_PTR                 route;
   IP_ROUTE_DIRECT_PTR              new_route = insertdata->route;
   uint_32                          i = insertdata->index;

   /* Make sure the node is initialized */
   if (!*node) {
      *node = node;
      _mem_zero(((IP_ROUTE_PRV_PTR)*node)->ROUTETYPE, sizeof(route->ROUTETYPE));
   } /* endif */

   route = *node;
   if (!route->ROUTETYPE[i]) {
      new_route->NEXT = new_route;
      route->ROUTETYPE[i] = new_route;
   } else if (insertdata->sort) {
      insertdata->sort(&route->ROUTETYPE[i], new_route);
   } else {
      new_route->NEXT = route->ROUTETYPE[i]->NEXT;
      route->ROUTETYPE[i]->NEXT = new_route;
   } /* Endif */

   insertdata->error = RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_delete
*  Returned Value : void
*  Comments       :
*        Removes a route from the IP routing table. Called by IPRADIX_insert.
*
*END*-----------------------------------------------------------------*/

struct ip_route_delete {
   uint_32              index;
   boolean (_CODE_PTR_  test)(pointer, pointer);
   pointer              data;
   IP_ROUTE_FN_PTR      fn;
};

static void IP_route_delete
   (
      pointer _PTR_  node,   /* ptr to address of the IPRADIX_NODE DATA ptr */
      pointer        data    /* ptr to data that can ID node to be deleted  */
   )
{ /* Body */
   IP_ROUTE_PRV_PTR                    route = *node;
   IP_ROUTE_DIRECT_PTR                 tmp_ptr, search_ptr;
   struct ip_route_delete _PTR_        remdata = data;
   uint_32                             i = remdata->index;
   IP_ROUTE_FN_PTR                     fn;

   if (route && route->ROUTETYPE[i]) {  /* Make sure the node is valid */

      /*
      ** Search the circular list for the correct node.
      ** tmp_ptr will always point to the node BEFORE search_ptr
      */
      tmp_ptr = route->ROUTETYPE[i];
      search_ptr = tmp_ptr->NEXT;
      do {

         if (remdata->test(search_ptr, remdata->data)) {
            tmp_ptr->NEXT = search_ptr->NEXT;    /* De-thread the node */
            if (route->ROUTETYPE[i] == search_ptr) {
               /* Update head if needed */
               route->ROUTETYPE[i] = search_ptr->NEXT;
            } /* Endif */
            if (route->ROUTETYPE[i] == search_ptr) {
               /* There is 1 node */
               route->ROUTETYPE[i] = NULL;
               if (route->ROUTETYPE[0] == NULL &&
                   route->ROUTETYPE[1] == NULL &&
                   route->ROUTETYPE[2] == NULL)
               {
                  *node = NULL;                  /* Free data ptr if needed */
               } /* Endif */
            } /* Endif */
            for (fn = remdata->fn; fn; fn = fn->NEXT) {
               fn->REM_RT((IP_ROUTE_INDIRECT_PTR)search_ptr);
            } /* Endfor */
            RTCS_part_free(search_ptr);
            break;
         } /* Endif */

         /* Move on to the next node */
         tmp_ptr = search_ptr;
         search_ptr = search_ptr->NEXT;
      } while (search_ptr != route->ROUTETYPE[i]->NEXT);
   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_add_direct
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Adds a direct route to the IP routing table. Assumes the
*        address, netmask and interfaces are valid.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_route_add_direct
   (
      _ip_address    address,    /* Address or network address of destif   */
      _ip_address    netmask,    /* Mask for the address parameter         */
      IP_IF_PTR      netif,      /* Interface for incomming packets        */
      IP_IF_PTR      destif      /* Interface for outgoing packets         */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR             IP_cfg_ptr = RTCS_getcfg(IP);
   IP_ROUTE_DIRECT_PTR           route;
   IP_ROUTE_FN_PTR               fn;
   struct ip_route_insert        insertdata;

   route = RTCS_part_alloc(IP_cfg_ptr->ROUTE_PARTID);
   if (!route) {
      return RTCSERR_IP_ROUTE_ALLOC;
   } /* Endif */

   route->ADDRESS       = address;
   route->NETIF         = netif;
   route->DESTIF        = destif;
   route->FLAGS         = RTF_STATIC | RTF_UP;

   insertdata.error = RTCSERR_IP_ROUTE_ALLOC;
   insertdata.route = route;
   insertdata.index = 0;
   insertdata.sort = NULL;

   IPRADIX_insert(&IP_cfg_ptr->ROUTE_ROOT.NODE, address & netmask, netmask,
                  IP_cfg_ptr->RADIX_PARTID,
                  IP_route_insert, &insertdata);

   if (insertdata.error) {
      RTCS_part_free(route);
   } else if (!~netmask) {
      ROUTE_new_bindif(route);   /* If the route is a leaf (has a full IP) */
   } else if (route->DESTIF != RTCS_IF_LOCALHOST_PRV) {
      fn = IP_cfg_ptr->ROUTE_FN; /* If the route is a network connection */
      while (fn) {
         fn->INIT_RT((IP_ROUTE_INDIRECT_PTR)((void _PTR_)route), 0);
         fn = fn->NEXT;
      } /* Endwhile */
   } /* Endif */

   return insertdata.error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_remove_direct
*  Returned Value : void
*  Comments       :
*        Removes a direct route from the IP routing table, if the node
*        is present.
*
*END*-----------------------------------------------------------------*/

struct ip_route_remove_direct {
   _ip_address          ipaddress;
   IP_IF_PTR            netif;
};

static boolean IP_route_delete_direct_test
   (
      pointer     routeptr,
      pointer     dataptr
   )
{ /* Body */
   IP_ROUTE_DIRECT_PTR                    route = routeptr;
   struct ip_route_remove_direct _PTR_    data  = dataptr;

   return (route->ADDRESS == data->ipaddress && route->NETIF == data->netif);
} /* Endbody */

void IP_route_remove_direct
   (
      _ip_address    address,    /* IP address of route to remove    */
      _ip_address    netmask,    /* IP netmask of route to remove    */
      IP_IF_PTR      netif       /* pointer to the route interface   */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR                IP_cfg_ptr = RTCS_getcfg(IP);
   struct ip_route_remove_direct    removedata;
   struct ip_route_delete           deldata;

   removedata.ipaddress = address;
   removedata.netif     = netif;

   deldata.data = &removedata;
   deldata.index = 0;
   deldata.test = IP_route_delete_direct_test;
   deldata.fn = IP_cfg_ptr->ROUTE_FN;
  
   IPRADIX_insert(&IP_cfg_ptr->ROUTE_ROOT.NODE, address & netmask, netmask,
                  0, IP_route_delete, &deldata);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_add_indirect
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Adds an indirect route to the IP routing table. Assumes the
*        address and netmask are valid. The caller has to set flag to
*        RTF_STATIC if ipif_bind or RTF_REDIRECT if icmpredirect.
*
*END*-----------------------------------------------------------------*/

static void IP_route_add_indirect_sort
   (
      pointer       head,
      pointer       new_node
   )
{ /* Body */
   IP_ROUTE_INDIRECT_PTR _PTR_      head_ptr = head;
   IP_ROUTE_INDIRECT_PTR            node = new_node, search = *head_ptr, last;
   IP_CFG_STRUCT_PTR                IP_cfg_ptr = RTCS_getcfg(IP);
   IP_ROUTE_FN_PTR                  fn;

   search = *head_ptr;

   /*
   ** Check if there is non static route here. Remove it if there is because
   ** static routes take precedence over dynamic routes. This function is only
   ** called if there is already a node, and if this node is a dynamic node
   ** there will only be one. We can simply replace it with the new node.
   */
   if (!(search->FLAGS & RTF_STATIC)) {
      for (fn = IP_cfg_ptr->ROUTE_FN; fn; fn = fn->NEXT) {
         fn->REM_RT(search);
      } /* Endfor */
      RTCS_part_free(search);
      *head_ptr = node;
      return;
   } /* Endif */

   /* Find the place to insert the new node */
   last = NULL;
   while (search->METRIC < node->METRIC) {
      last = search;
      search = search->NEXT;
      if (search == *head_ptr) {
         break;
      } /* Endif */
   } /* Endwhile */

   /*
   ** At this point we found the place to insert. We need to special case
   ** insertion into the front of the list, but the other cases are easy.
   */
   if (!last) {
      /*
      ** The new node has to be the first in the list. We need a pointer to the
      ** last element in the list to complete the insertion. The following
      ** insertion works for any number of nodes.
      */
      last = search->NEXT;
      while (last->NEXT != search) {
         last = last->NEXT;
      } /* Endwhile */

      /* Update the head_ptr to point to the new node */
      *head_ptr = node;
   } /* Endif */

   /* Link in the new node */
   node->NEXT = search;
   last->NEXT = node;

   return;
} /* Endbody */

#if RTCSCFG_ENABLE_GATEWAYS
uint_32 IP_route_add_indirect
   (
      _ip_address    address,    /* Gateway address */
      _ip_address    netmask,    /* Network mask    */
      _ip_address    network,    /* Network address */
      uint_32        flag,       /* [IN] RTF_* */
      uint_16        metric      /* [IN] the route metric [0,65535] */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR                IP_cfg_ptr = RTCS_getcfg(IP);
   IP_ROUTE_INDIRECT_PTR            route;
   struct ip_route_insert           insertdata;
   IP_ROUTE_FN_PTR                  fn;

   route = RTCS_part_alloc(IP_cfg_ptr->GATE_PARTID);
   if (!route) {
      return RTCSERR_IP_GATE_ALLOC;
   } /* Endif */

   route->GATEWAY    = address;
   route->FLAGS      = flag | RTF_UP;
   route->IS_DIRECT  = NULL;
   route->METRIC     = metric;

   insertdata.error = RTCSERR_IP_GATE_ALLOC;
   insertdata.route = route;
   insertdata.index = 1;
   insertdata.sort = IP_route_add_indirect_sort;

   IPRADIX_insert(&IP_cfg_ptr->ROUTE_ROOT.NODE, network & netmask, netmask,
                  IP_cfg_ptr->RADIX_PARTID,
                  IP_route_insert, &insertdata);

   if (insertdata.error) {
      RTCS_part_free(route);
   } else  {
      fn = IP_cfg_ptr->ROUTE_FN;
      while (fn) {
         fn->INIT_RT(route, metric);
         fn = fn->NEXT;
      } /* Endwhile */
   } /* Endif */

   return insertdata.error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_remove_indirect
*  Returned Value : void
*  Comments       :
*        Removes one or all indirect routes from the IP routing table,
*        if the node is present.
*
*END*-----------------------------------------------------------------*/

static void IP_route_delete_indirect_all
   (
      pointer _PTR_  node,   /* ptr to address of the IPRADIX_NODE DATA ptr */
      pointer        data    /* ptr to the gateway address to be removed    */
   )
{ /* Body */
   IP_ROUTE_PTR            route = *node;
   IP_ROUTE_INDIRECT_PTR   tmp_ptr, search_ptr;
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg(IP);
   IP_ROUTE_FN_PTR         fn;

   if (route && route->INDIRECT) {  /* Make sure the node is valid */

      search_ptr = route->INDIRECT->NEXT; /* search with next element */
      tmp_ptr = search_ptr->NEXT;
      /* if we  have only one elment while loop will not execute */
      while (search_ptr != route->INDIRECT) {
         for (fn = IP_cfg_ptr->ROUTE_FN; fn; fn = fn->NEXT) {
            fn->REM_RT(search_ptr);
         } /* Endfor */
         RTCS_part_free(search_ptr);
         search_ptr = tmp_ptr;
         tmp_ptr = search_ptr->NEXT;
      } /* Endwhile */
      /* Free the first element */
      for (fn = IP_cfg_ptr->ROUTE_FN; fn; fn = fn->NEXT) {
         fn->REM_RT(search_ptr);
      } /* Endfor */
      RTCS_part_free(search_ptr);
      if (route->DIRECT == NULL && route->VIRTUAL == NULL) {
         *node = NULL;                   /* Free data ptr if needed */
      } else {
         route->INDIRECT = NULL;
      } /* Endif */
   } /* Endif */

} /* Endbody */

static boolean IP_route_delete_indirect_test
   (
      pointer         routeptr,
      pointer         dataptr
   )
{ /* Body */
   IP_ROUTE_INDIRECT_PTR     route = routeptr;
   struct {
      _ip_address       gate;
      uint_32           flag;
      uint_16           metric;
   } _PTR_ testdata = dataptr;

   return (route->GATEWAY == testdata->gate &&
           (route->FLAGS & testdata->flag) == testdata->flag &&
           route->METRIC == testdata->metric);
} /* Endbody */

void IP_route_remove_indirect
   (
      _ip_address    gateway,    /* Gateway address   */
      _ip_address    netmask,    /* Network mask      */
      _ip_address    network,    /* Network address   */
      uint_32        flag,       /* [IN] RTF_* */
      uint_16        metric      /* [IN] the route metric [0,65535] */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg(IP);
   struct ip_route_delete  deldata;
   struct {
      _ip_address       gate;
      uint_32           flag;
      uint_16           metric;
   } testdata;

   if (gateway) {
      testdata.gate = gateway;
      testdata.flag = flag;
      testdata.metric = metric;

      deldata.index = 1;
      deldata.data = &testdata;
      deldata.test = IP_route_delete_indirect_test;
      deldata.fn = IP_cfg_ptr->ROUTE_FN;

      IPRADIX_insert(&IP_cfg_ptr->ROUTE_ROOT.NODE, network & netmask, netmask,
                    0, IP_route_delete, &deldata);
   } else {
      IPRADIX_insert(&IP_cfg_ptr->ROUTE_ROOT.NODE, network & netmask, netmask,
                     0, IP_route_delete_indirect_all, &gateway);
   } /* Endif */

} /* Endbody */
#endif

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_add_virtual
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Adds a virtual route to the IP routing table. Assumes the
*        address, netmask and interfaces are valid.
*
*END*-----------------------------------------------------------------*/

uint_32 IP_route_add_virtual
   (
      _ip_address   address,        /* Destination address              */
      _ip_address   netmask,        /* Mask for the address parameter   */
      _ip_address   source,         /* Source address for interface     */
      _ip_address   source_net,     /* Allowed source network           */
      _ip_address   source_mask,    /* Allowes source network mask      */
      IP_IF_PTR     destif,         /* Interface for outgoing packets   */
      pointer       data            /* Route information                */
   )
{ /* Body */
#if RTCSCFG_ENABLE_VIRTUAL_ROUTES
   IP_CFG_STRUCT_PTR                IP_cfg_ptr = RTCS_getcfg(IP);
   IP_ROUTE_VIRTUAL_PTR             route;
   struct ip_route_insert           insdata;
   IP_ROUTE_FN_PTR                  fn;

   route = RTCS_part_alloc(IP_cfg_ptr->VIRTUAL_PARTID);
   if (!route) {
      return RTCSERR_IP_VIRTUAL_ALLOC;
   } /* Endif */

   route->ADDRESS       = source;
   route->IS_DIRECT     = NULL;
   route->DESTIF        = destif;
   route->FLAGS         = RTF_STATIC | RTF_UP;
   route->SOURCE_NET    = source_net;
   route->SOURCE_MASK   = source_mask;
   route->DATA          = data;

   insdata.error = RTCSERR_IP_VIRTUAL_ALLOC;
   insdata.route = route;
   insdata.index = 2;
   insdata.sort = NULL;

   IPRADIX_insert(&IP_cfg_ptr->ROUTE_ROOT.NODE, address & netmask, netmask,
                  IP_cfg_ptr->RADIX_PARTID, IP_route_insert, &insdata);

   if (insdata.error) {
      RTCS_part_free(route);
   } else if (!~netmask) {
      fn = IP_cfg_ptr->ROUTE_FN;
      while (fn) {
         fn->INIT_RT((IP_ROUTE_INDIRECT_PTR)route, 0);
         fn = fn->NEXT;
      } /* Endwhile */
   } /* Endif */

   return insdata.error;
#else
      return RTCSERR_IP_VIRTUAL_ALLOC;
#endif
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_remove_virtual
*  Returned Value : void
*  Comments       :
*        Removes a virtual route from the IP routing table, if the node
*        is present.
*
*END*-----------------------------------------------------------------*/

struct ip_route_remove_virtual {
   _ip_address          source;
   _ip_address          source_net;
   _ip_address          source_mask;
   IP_IF_PTR            netif;
};

static boolean IP_route_delete_virtual_test
(
      pointer    routeptr,
      pointer    dataptr
   )
{ /* Body */
   IP_ROUTE_VIRTUAL_PTR                    route = routeptr;
   struct ip_route_remove_virtual _PTR_    data  = dataptr;

   return (route->ADDRESS == data->source &&
           route->SOURCE_NET == data->source_net &&
           route->SOURCE_MASK == data->source_mask &&
           route->DESTIF == data->netif);
} /* Endbody */

void IP_route_remove_virtual
   (
      _ip_address    address,       /* IP address of route to remove    */
      _ip_address    netmask,       /* IP netmask of route to remove    */
      _ip_address    source,        /* IP of interface                  */
      _ip_address    source_net,    /* Allowed source network           */
      _ip_address    source_mask,   /* Allowed source source netmak     */
      IP_IF_PTR      netif          /* pointer to the route interface   */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR                 IP_cfg_ptr = RTCS_getcfg(IP);
   struct ip_route_remove_virtual    removedata;
   struct ip_route_delete            deldata;

   removedata.source       = source;
   removedata.source_net   = source_net;
   removedata.source_mask  = source_mask;
   removedata.netif        = netif;

   deldata.data = &removedata;
   deldata.index = 2;
   deldata.test = IP_route_delete_virtual_test;
   deldata.fn = IP_cfg_ptr->ROUTE_FN;

   IPRADIX_insert(&IP_cfg_ptr->ROUTE_ROOT.NODE, address & netmask, netmask,
                  0, IP_route_delete, &deldata);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_local
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Decides whether or not to keep a packet.
*
*END*-----------------------------------------------------------------*/

struct ip_route_local_test {
   IP_IF_PTR   ifdest;
   _ip_address ipdest;
   RTCSPCB_PTR pcb;
};

static boolean IP_route_local_test
   (
      pointer  node,
      pointer  data
   )
{ /* Body */
   struct ip_route_local_test _PTR_ testdata = data;
   IP_ROUTE_PTR                     route = node;
   IP_ROUTE_DIRECT_PTR              search_ptr = route->DIRECT;
   _ip_address                      ip_bcast;

   /* Don't process IP unicast packets without link-layer unicast bit set */
   if (!~route->NODE.MASK && !(testdata->pcb->TYPE & RTCSPCB_TYPE_UNICAST)) {
      return TRUE;
   } /* Endif */

#if RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST
   /*
   ** This node matches a unicast address if MASK == 0xFFFFFFFF, and a
   ** directed broadcast if MASK != 0xFFFFFFFF.  Here we return IP_UNREACH
   ** for directed broadcast packets.
   */
   if (~route->NODE.MASK) {
      return TRUE;
   } /* Endif */
#endif

   /* Calculate this route's broadcast address */
   ip_bcast = route->NODE.IP | ~route->NODE.MASK;

   if (search_ptr) {
      do {
         /*
         ** 1. Only check interfaces on which the packet was received, unless
         **    IP_forwarding is enabled.
         ** 2. Accept broadcast, or unicast packets with destinations that
         **    match node address.
         */
         if ((testdata->pcb->IFSRC == search_ptr->NETIF || _IP_forward) &&
             (testdata->ipdest == ip_bcast))
         {
            testdata->ifdest = search_ptr->DESTIF;
            return TRUE;
         } /* Endif */

         search_ptr = search_ptr->NEXT;
      } while (search_ptr != route->DIRECT);
   } /* Endif */

   return FALSE;
} /* Endbody */

uint_32 IP_route_local
   (
      RTCSPCB_PTR    pcb,
            /* [IN] the packet to send */
      _ip_address    ipdest
            /* [IN] the ultimate destination */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR          IP_cfg_ptr = RTCS_getcfg(IP);
   struct ip_route_local_test testdata;

   /* 
   ** RFC 2131 says:
   ** "In the case of a client using DHCP for initial configuration (before
   ** the client's TCP/IP software has been completely configured), DHCP
   ** requires creative use of the client's TCP/IP software and liberal
   ** interpretation of RFC 1122.  The TCP/IP software SHOULD accept and
   ** forward to the IP layer any IP packets delivered to the client's
   ** hardware address before the IP address is configured."
   **
   ** So if the broadcast flag is disabled, we check, and if the packet's 
   ** source interface is not bound, we send the packet to IF_LOCALHOST.
   ** This is paired with the same test in ip.c:IP_service(), which sends 
   ** the packet here.
   */
   if ((ipdest == INADDR_BROADCAST) 
       || IN_MULTICAST(ipdest)
       || (!_DHCP_broadcast && (IP_get_ipif_addr(pcb->IFSRC) == INADDR_ANY))) { 
      /* Accept broadcasts and multicasts and unbound unicasts */
      testdata.ifdest = RTCS_IF_LOCALHOST_PRV;
   } else {

      /* If unicast, check routing table */
      testdata.ifdest = NULL;
      testdata.ipdest = ipdest;
      testdata.pcb    = pcb;
      IPRADIX_findbest(&IP_cfg_ptr->ROUTE_ROOT.NODE, ipdest,
                       IP_route_local_test, &testdata);
   } /* Endif */

   /* Send the packet to the interface. */
   if (testdata.ifdest) {
      return IP_send_dgram( RTCS_IF_LOCALHOST_PRV, pcb, 0, ipdest, ipdest, 0, NULL);
   } /* Endif */

   /* No interface was found. Discard the packet */
   IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
   IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.ST_RX_ADDR_ERRORS++);
   RTCSLOG_PCB_FREE(pcb, RTCSERR_IP_UNREACH);
   RTCSPCB_free(pcb);
   return RTCSERR_IP_UNREACH;

} /* Endbody */





/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route_multi
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Chooses one or more routes for a multicast or limited broadcast
*        packet and sends it.  Note:  at the very least, the packet will
*        be sent up to the localhost's transport layer.
*
*END*-----------------------------------------------------------------*/

struct ip_route_multi_struct {
   RTCSPCB_PTR pcb;
   uint_32     protocol;
   _ip_address ipsrc;
   _ip_address ipdest;
   uint_32     flags;
};

static boolean IP_route_multi_test
   (
      _ip_address node_ip,
      _ip_address node_mask,
      pointer     node_data,
      pointer     data
   )
{ /* Body */
   IP_CFG_STRUCT_PTR                   IP_cfg_ptr = RTCS_getcfg(IP);
   struct ip_route_multi_struct _PTR_  testdata = data;
   IP_ROUTE_PTR                        route = node_data;
   IP_ROUTE_DIRECT_PTR                 direct;
   IP_ROUTE_VIRTUAL_PTR                virtual;
   _ip_address                         ip_bcast;
   RTCSPCB_PTR                         dup_pcb;

   if (!route) {
      return FALSE;
   } /* Endif */

   /* Calculate this route's broadcast address */
   ip_bcast = node_ip | ~node_mask;

   direct = route->DIRECT;
   if (direct) {
      do {
         /* Note:
         ** 1. The packet is sent to LOCALHOST in IP_route_multi.
         ** 2. The source address must match one of the interface's address(es).
         ** 3. If the source address is broadcast or if it is specified, use
         **    this interface.
         */
         if (direct->DESTIF != RTCS_IF_LOCALHOST_PRV &&
             (testdata->ipsrc == INADDR_ANY ||
              testdata->ipsrc == INADDR_BROADCAST ||
              testdata->ipsrc == ip_bcast ||
              testdata->ipsrc == direct->ADDRESS))
         {
            /*
            ** Make sure not to send the packet back to the interface it was
            ** received on.
            */
            if (!(testdata->flags & IPROUTEOPT_RECVIF) ||
               (testdata->pcb->IFSRC != direct->DESTIF))
            {
               /* We've chosen an interface. Duplicate the PCB and send it */
               dup_pcb = RTCSPCB_alloc_dup(testdata->pcb);
               if (dup_pcb) {
                  //RTCSLOG_PCB_ALLOC(dup_pcb);
                  IP_send_dgram(direct->DESTIF, dup_pcb, direct->ADDRESS,
                     testdata->ipdest, testdata->ipdest, testdata->protocol,
                     NULL);
               } else {
                  IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
               } /* Endif */
            } /* Endif */
         } /* Endif */

         direct = direct->NEXT;
      } while (direct != route->DIRECT);
   } /* Endif */

   virtual = route->VIRTUAL;
   if (virtual) {
      do {
         /*
         ** This section is similar to the direct route case above.
         ** However, we only want to send to numbered interfaces.
         */
         if (virtual->DESTIF != RTCS_IF_LOCALHOST_PRV &&
             virtual->ADDRESS &&
             (testdata->ipsrc == INADDR_ANY ||
              testdata->ipsrc == INADDR_BROADCAST ||
              testdata->ipsrc == virtual->ADDRESS))
         {
            /*
            ** Make sure not to send the packet back to the interface it was
            ** received on.
            */
            if (!(testdata->flags & IPROUTEOPT_RECVIF) ||
               (testdata->pcb->IFSRC != virtual->DESTIF))
            {
               /* We've chosen an interface. Duplicate the PCB and send it */
               dup_pcb = RTCSPCB_alloc_dup(testdata->pcb);
               if (dup_pcb) {
                  //RTCSLOG_PCB_ALLOC(dup_pcb);
                  IP_send_dgram(virtual->DESTIF, dup_pcb, virtual->ADDRESS,
                     testdata->ipdest, testdata->ipdest, testdata->protocol,
                     NULL);
               } else {
                  IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
               } /* Endif */
            } /* Endif */
         } /* Endif */

         virtual = virtual->NEXT;
      } while (virtual != route->VIRTUAL);
   } /* Endif */

   return FALSE;
} /* Endbody */

uint_32 IP_route_multi
   (
      RTCSPCB_PTR    pcb,
            /* [IN] the packet to send */
      uint_32        protocol,
            /* [IN] the transport layer protocol */
      _ip_address    ipsrc,
            /* [IN] the destination interface (0 = any) */
      _ip_address    ipdest,
            /* [IN] the ultimate destination */
      uint_32        flags
            /* [IN] optional flags */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR             IP_cfg_ptr = RTCS_getcfg(IP);
   struct ip_route_multi_struct  testdata;

   testdata.pcb      = pcb;
   testdata.protocol = protocol;
   testdata.ipsrc    = ipsrc;
   testdata.ipdest   = ipdest;
   testdata.flags    = flags;

   IPRADIX_walk(&IP_cfg_ptr->ROUTE_ROOT.NODE, IP_route_multi_test, &testdata);

   /*
   ** At this point, we've sent the packet out each interface it's
   ** intended for.  Only the local host is left.
   */

   if ((flags & IPROUTEOPT_RECVIF) &&
      (pcb->IFSRC == RTCS_IF_LOCALHOST_PRV))
   {
      RTCSLOG_PCB_FREE(pcb, RTCS_OK);
      RTCSPCB_free(pcb);
      return RTCS_OK;
   } /* Endif */

   return IP_send_dgram( RTCS_IF_LOCALHOST_PRV, pcb, INADDR_LOOPBACK,
      ipdest, ipdest, protocol, NULL);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IP_route
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Chooses a route for a packet and sends it.  Note:  this
*        includes sending the packet up to the localhost's transport
*        layer (by choosing ifdest == RTCS_IF_LOCALHOST_PRV).
*
*END*-----------------------------------------------------------------*/

struct ip_route_struct {
   RTCSPCB_PTR pcb;
   _ip_address ipsrc;
   _ip_address ipdest;
   uint_32     flags;

   uint_32     retval;
   boolean     check_gateway;
   IP_IF_PTR   ifdest;
   _ip_address hopdest;
   _ip_address hopsrc;
   RTCSPCB_PTR keep_pcb;

   boolean     sendit;   /* Should we send out the packet ? */
   boolean     keepit;   /* Should we send a copy to the localhost ? */
   boolean     hopsrcislocal;
   pointer     data;     /* Routing entry data */
};

static boolean IP_route_test
   (
      pointer     node_data,
      pointer     data
   )
{ /* Body */
   IP_CFG_STRUCT_PTR                IP_cfg_ptr = RTCS_getcfg(IP);
   struct ip_route_struct _PTR_     testdata = data;
   IP_ROUTE_PTR                     route = node_data;
   IP_ROUTE_DIRECT_PTR              direct = route->DIRECT;
   IP_ROUTE_INDIRECT_PTR            indirect = route->INDIRECT;
   IP_ROUTE_VIRTUAL_PTR             virtual = route->VIRTUAL;
   _ip_address                      ip_bcast, source;

   if (direct) {

      /* Calculate this route's broadcast address */
      ip_bcast = route->NODE.IP | ~route->NODE.MASK;

      /* Reject a packet with a destination of 0 for the host IP */
      if (~route->NODE.MASK && testdata->ipdest == route->NODE.IP) {
         IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_DISCARDED++);
         return TRUE;
      } /* Endif */

      do {
         /* source address must match (one of) the interface's address(es) */
         if (testdata->hopsrc == INADDR_ANY||
             testdata->hopsrc == INADDR_BROADCAST ||
             testdata->hopsrc == ip_bcast ||
             testdata->hopsrc == direct->ADDRESS ||
             _IP_forward)
         {
            /*
            ** At this point we've chosen an interface.
            ** Now decide whether destination is unicast or
            ** directed broadcast.
            */

            /* Use this interface's source address if we don't have one */
            if (!testdata->hopsrcislocal) {
               testdata->hopsrc = direct->ADDRESS;
            } /* Endif */

            /*
            ** Decide if we should send a copy to the local host. We want to
            ** send a copy to the local host if it is a directed broadcast.
            */
            testdata->keepit = (~route->NODE.MASK && testdata->ipdest == ip_bcast);
            testdata->sendit = TRUE;

#if RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST
            if (testdata->keepit) {
               /*
               ** If this is a directed broadcast, discard it.  Don't even
               ** keep a copy for the localhost.
               */
               testdata->keepit = FALSE;
               testdata->sendit = FALSE;
            } /* Endif */
#endif

            /*
            ** IPROUTEOPT_RECVIF means that we must not send the packet
            ** out the interface it was received on.  This is turned on
            ** when we receive a broadcast (because everyone on that
            ** network already received a copy), and also when originating
            ** a packet with the MSG_NOLOOP flag.
            */
            if (testdata->flags & IPROUTEOPT_RECVIF) {
               if (testdata->keepit) {
                  /*
                  ** If broadcast packet, set hopdest to INADDR_BROADCAST. 
                  */
                  testdata->hopdest = INADDR_BROADCAST;
               } /* Endif */
               testdata->keepit = testdata->keepit && (testdata->pcb->IFSRC != RTCS_IF_LOCALHOST_PRV);
               testdata->sendit = testdata->sendit && (testdata->pcb->IFSRC != direct->DESTIF);
            } /* Endif */

            /*
            ** This #if is purely for code size reduction.  If directed
            ** broadcasts are disabled, keepit will always be false.
            */
#if !RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST

            if (testdata->keepit) {

               /*
               ** If keepit and sendit:
               **
               ** We will need two PCB's. One to send out of the interface and
               ** one to send up to the localhost. The packet is being sent to both
               ** interfaces because it is a broadcast packet, so we set hopdest to
               ** INADDR_BROADCAST. We also set the destif to the network interface
               ** and we don't bother saving a pointer to the locahost interface
               ** because it is already accessible in the IP_CFG_STRUCT.
               */
               if (testdata->sendit) {
                  testdata->ifdest = direct->DESTIF;
                  testdata->hopdest = INADDR_BROADCAST;

                  testdata->keep_pcb = RTCSPCB_alloc_dup(testdata->pcb);
                  if (!testdata->keep_pcb) {
                     IF_IP_STATS_ENABLED(IP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
                  } /* Endif */
               } /* Endif */

               /* If keepit and !sendit:
               **
               ** This means that a direct broadcast was received, and the network
               ** it was received from is the destination network.
               ** eg: The packet is received from an interface connected to a
               ** network 192.168.1.0 and the packet destination is 192.168.1.255
               ** If it was received on its destination network everybody on that
               ** network also received a copy. There is no reason to send it back
               ** out, so we send it to the localhost only.
               ** The packet was a broadcast packet, so hopdest = INADDR_BROADCAST.
               ** But IP_route will know this if keepit is true, so we don't bother
               ** setting hopdest.
               */
               else {
                  testdata->keep_pcb = testdata->pcb;
               } /* Endif */

            } else
#endif

            /* If !keepit and sendit:
            **
            ** This is the case when we receive a unicast packet. We simply send
            ** it out of the chosen interface (set ifdest). We could be sending
            ** this packet directly to the destination, or we could be sending
            ** it to a gateway. If we are sending it to a gateway, hopdest is
            ** already set to the address of the gateway, so we don't modify it.
            ** Otherwise, if we are sending directly to the destination, set
            ** hopdest to the destination IP.
            */
            if (testdata->sendit) {
               testdata->ifdest = direct->DESTIF;
               if (!testdata->hopdest) {
                  testdata->hopdest = testdata->ipdest;
               } /* Endif */
            } /* Endif */

            /* If !keepit and !sendit:
            **
            ** Do nothing, and IP_route will silently discard the packet.
            */
            else { } /* Endif */

            /* Use a different interface next time */
            route->DIRECT = route->DIRECT->NEXT;
            testdata->retval = RTCS_OK;
            return TRUE;
         } /* Endif */

         direct = direct->NEXT;
      } while(direct != route->DIRECT);
   } /* Endif */

   /*
   ** Reaching this point means that the destination isn't directly connected
   ** to us. Check for a virtual route (tunnel, unnumbered interface or other).
   ** Only send packets out of interfaces, never directly to localhost
   */

   if (virtual && !(testdata->flags & IPROUTEOPT_NOVIRTUAL)) {

      /*
      ** If we are forwarding the packet, ipsrc is set to the source address
      ** of the IP header. If we are originating a packet, hopsrc is set to the
      ** IP address of the local originating interface
      */
      source = testdata->ipsrc ? testdata->ipsrc : testdata->hopsrc;

      do {
         /*
         ** The following THREE conditions must be TRUE:
         **
         ** 1. One of the following:
         **       a) source address must be INADDR_ANY (means pick the source of
         **          the interface), OR
         **       b) source address must be INADDR_BROADCAST, in which case the
         **          IP source address becomes the source of the interface, OR
         **       c) the source address matches the interface address, OR
         **       d) IP forwarding is enabled
         **
         ** 2. If we are not forwarding a packet, the local hop source must be
         **    specified (hopsrc != INADDR_ANY) or the interface must have a
         **    source address (which we will use when we have no hopsrc).
         **    NOTE: (testdata->ipsrc != INADDR_ANY) means we are forwarding
         **
         ** 3. The source IP of the packet must be within the source network
         **    allowed by the interface
         */
         if ((testdata->hopsrc == INADDR_ANY ||
              testdata->hopsrc == INADDR_BROADCAST ||
              testdata->hopsrc == virtual->ADDRESS ||
              _IP_forward) &&
             (virtual->ADDRESS || testdata->ipsrc || testdata->hopsrcislocal) &&
             ((source & virtual->SOURCE_MASK) == virtual->SOURCE_NET))
         {
            /*
            ** At this point we've chosen an interface.
            */

            /* Use this interface's source address if we don't have one */
            if (!testdata->hopsrcislocal) {
               testdata->hopsrc = virtual->ADDRESS;
               if (!testdata->hopsrc) {
                  testdata->hopsrc = testdata->ipsrc;
               } /* Endif */
            } /* Endif */

            /*
            **
            ** This is the case when we receive a unicast packet. We simply send
            ** it out of the chosen interface (set ifdest). We could be sending
            ** this packet directly to the destination, or we could be sending
            ** it to a gateway. If we are sending it to a gateway, hopdest is
            ** already set to the address of the gateway, so we don't modify it.
            ** Otherwise, if we are sending directly to the destination, set
            ** hopdest to the destination IP.
            */

            testdata->ifdest = virtual->DESTIF;
            if (!testdata->hopdest) {
               testdata->hopdest = testdata->ipdest;
            } /* Endif */

            /* Use a different interface next time */
            route->VIRTUAL = route->VIRTUAL->NEXT;
            testdata->sendit = TRUE;
            testdata->retval = RTCS_OK;
            testdata->data = virtual->DATA;
            return TRUE;
         } /* Endif */

         virtual = virtual->NEXT;
      } while(virtual != route->VIRTUAL);

   } /* Endif */

   /*
   ** Reaching this point means that the destination isn't directly connected
   ** to us. Check for a gateway (if we haven't done so already).
   */

   if (indirect && testdata->check_gateway) {
      do {
          if (indirect->FLAGS & RTF_UP) {
            testdata->hopdest = indirect->GATEWAY;

            /* Notify calling function that a gateway was found */
            testdata->check_gateway = FALSE;

            return TRUE;
          } /* Endif */

         indirect = indirect->NEXT;
      } while(indirect != route->INDIRECT);
   } /* Endif */

   return FALSE;
} /* Endbody */


uint_32 IP_route
   (
      RTCSPCB_PTR    pcb,
            /* [IN] the packet to send */
      uint_32        protocol,
            /* [IN] the transport layer protocol */
      _ip_address    hopsrc,
            /* [IN] the destination interface (0 = any) */
      _ip_address    ipsrc,
            /* [IN]the ultimate source */
      _ip_address    ipdest,
            /* [IN] the ultimate destination */
      uint_32        flags
            /* [IN] optional flags */
   )
{ /* Body */
   IP_CFG_STRUCT_PTR       IP_cfg_ptr = RTCS_getcfg(IP);
   struct ip_route_struct  testdata;
   
   RTCSLOG_FNE4(RTCSLOG_FN_IP_route, pcb, protocol, hopsrc);

   testdata.pcb            = pcb;
   testdata.ipsrc          = ipsrc;
   testdata.ipdest         = ipdest;
   testdata.flags          = flags;

   testdata.retval         = RTCSERR_IP_UNREACH;
   testdata.check_gateway  = TRUE;
   testdata.ifdest         = NULL;
   testdata.hopdest        = INADDR_ANY;
   testdata.keep_pcb       = NULL;

   testdata.keepit         = FALSE;
   testdata.sendit         = FALSE;
   testdata.hopsrc         = hopsrc;
   testdata.hopsrcislocal  = IP_is_local(NULL, hopsrc);
   testdata.data           = NULL;

   IPRADIX_findbest(&IP_cfg_ptr->ROUTE_ROOT.NODE, ipdest, IP_route_test, &testdata);

   /* If we have found a gateway, find an interface for the gateway */
   if (!testdata.check_gateway) {
      IPRADIX_findbest(&IP_cfg_ptr->ROUTE_ROOT.NODE, testdata.hopdest,
         IP_route_test, &testdata);
   } /* Endif */

   /* Discard the packet */
   if (testdata.retval || (!testdata.keepit && !testdata.sendit)) {
      RTCSLOG_PCB_FREE(pcb, testdata.retval);
      RTCSPCB_free(pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_IP_route, testdata.retval);
      return testdata.retval;
   } /* Endif */

   /* Should we send to localhost? */
   if (testdata.keepit && testdata.keep_pcb) {
      testdata.retval = IP_send_dgram( RTCS_IF_LOCALHOST_PRV,
         testdata.keep_pcb, testdata.hopsrc, INADDR_BROADCAST, ipdest, protocol,
         NULL);
   } /* Endif */

   /* Should we send it out? */
   if (testdata.sendit) {
      testdata.retval = IP_send_dgram(testdata.ifdest, pcb, testdata.hopsrc,
         testdata.hopdest, ipdest, protocol, testdata.data);
   } /* Endid */

   RTCSLOG_FNX2(RTCSLOG_FN_IP_route, testdata.retval);
   return testdata.retval;
} /* Endbody */


#endif /* RTCSCFG_ENABLE_IP4 */


/* EOF */
