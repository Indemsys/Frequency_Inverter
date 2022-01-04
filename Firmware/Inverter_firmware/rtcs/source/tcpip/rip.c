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
* $FileName: rip.c$
* $Version : 3.8.12.0$
* $Date    : Sep-19-2012$
*
* Comments:
*
*   This file contains the implementation of RIPv1 and v2,
*   the Routing Information Protocol.  For more details,
*   refer to RFC2453.
*   KNOWN BUGS:
*   - just after the shutdown advertise all the routes with an max (no infinite)
*   metric aka 15. thus the other routers doesnt have to wait for the end of
*   the timeout. question: how do i know when the board shutdown?
*   it seems impossible :( perhaps with a network call... snmp set
*   - how to do the router administration ? etc.. snmp set? and the security ?
*   - how to advertize particular route such as default. currently i dont
*   adertize it.
*   - passwd and md5 authentification extensions are not handled.
*   - i missed something in the nexthop field and the reverse poison, the
*   metric must be advertized with a infinite metric on the interface
*   from which it has been learned.
*   ok BUT only if this route has been learned with RIP. not if it is a external
*   route/no static route (because there is not source interface).
*   and perhaps other cases... like on demand circuit.
*   > > in which case the nexthop field is usefull ?
*   >
*   > One obvious case -- exporting from a foreign routing protocol (like
*   > OSPF or BGP).
*   >
*   > Another very useful case for the next hop field in RIPv2 is to allow
*   > for better handling of routes on demand circuits.  In the Annex, we
*   > put the next hop into the advertisement even if it wasn't directly
*   > reachable on that same subnet.  I think Gary wrote an applicability
*   > statement concerning this use afterwards.
*   >
*   > Suppose you have nodes A and B which can both dial into C on demand.
*   > A dials in and learns a number of routes from C and advertises them on
*   > a local network that B sees.  A hangs up from C and then B dials in.
*   >
*   > When B learns routes from C, it advertises C as the next hop.  When A
*   > sees this, it immediately sets its local copies of those routes to
*   > infinity and purges them from the world by doing a triggered
*   > advertisement.  That way, other hosts don't have to wait for A's old
*   > routes to age out and B's to become preferred.
*   TODO:
*   - the icmp/redirect route must be modified to.
*   - handle the case of removing the interface or the device.
*   - must handle several route for a given destination with the same metric.
*   - check response validity.
*   - if a route metric reaches the max, the route is down.
*   - the debian routed send the packet to the network broadcast and not
*   the limited broadcast, what is the proper behaviour ? gated routed do
*   the same. DONE i have decided it was the good one.
*   - if an static route is removed, start the deletion process. dont simply
*   remove it. thus the route is advertized with an infinite metric.
*   - implement the POLL unofficial/gated command. i.e. a response
*   without splithorizon/reverse poison.
*   - DONE send/received multicast for ripv2 (and ospf) (change igmp to do
*   it without socket. directly in ucb) seb said ok
*
*END************************************************************************/


/* local include */
#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"
#include "udp_prv.h"
#include "rip.h"
#include "route.h"
#if RTCSCFG_ENABLE_RIP && RTCSCFG_ENABLE_IP4 

#define RND_RANGE(min,max) ((min) + RTCS_rand()%(((max)-(min))?((max)-(min)):1))
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#define RIP_PKT_MAXLEN      500 /* find the right number */
#define RIP_AF_INET     2   /* MUST be 2. rtcs AF_INET is 1! */
#define DEFAULT_ROUTE_TAG       0
#define NETWORK_COST            1   /* must be at least 1 */
#define SPLIT_HORIZON_WITHOUT_POISONNED_REVERSE 0
#define ADV_VERSION     RIP_V2


static void RIP_trig_upd(void);
static boolean RIP_expire_periodic(TCPIP_EVENT_PTR);
static uint_32 RIP_send_req(void);
void RIP_service(RTCSPCB_PTR, UCB_STRUCT_PTR);

IP_ROUTE_FN RIP_routefn = {
   NULL,
   RIP_new_bindif,
   RIP_init_static_rt,
   RIP_remove_rt
};

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_init
* Returned Values : uint_32
* Comments        : Initialize the RIP layer.
*
*END*-----------------------------------------------------------------*/

uint_32 RIP_init (void)
{ /* Body */
    RIP_CFG_STRUCT_PTR  cfg = RTCS_getcfg(RIP);
    uint_32 err;

    if (RTCS_getcfg(RIP)) return RTCS_OK;
    /* allocate the memory */
    cfg = RTCS_mem_alloc_zero(sizeof(*cfg));
    if (!cfg)  return RTCSERR_OUT_OF_MEMORY;
    /* bind the udp port */
    err = UDP_openbind_internal(IPPORT_RIP, RIP_service, &cfg->UCB);
    if (err){
        _mem_free(cfg);
        return err;
    }

    RTCS_setcfg(RIP, cfg);
    ROUTE_register(&RIP_routefn);

    /* Start the retransmission timer to start sending immediately */
    cfg->TIMER_PERIODIC.TIME    = 0;
    cfg->TIMER_PERIODIC.EVENT   = RIP_expire_periodic;
    cfg->TIMER_PERIODIC.PRIVATE = NULL;
    TCPIP_Event_add(&cfg->TIMER_PERIODIC);

    /* probe all the interfaces now to build the route table faster */
    err = RIP_send_req();

    return RTCS_OK;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_cpu_metric
* Returned Values :
* Comments        : rfc2453.3.9.2
*
*END*-----------------------------------------------------------------*/

static uint_32 RIP_cpu_metric(
    RTCSPCB_PTR     pcb,          /* [IN] incoming packet */
    uint_32     oldmetric
)
{ /* Body */
    uint_32 metric = NETWORK_COST + oldmetric;
/* if a cost per interface is required, do that here. But before that,
** think about the unit problem between the routing protocols.
*/
    return min(metric, RIP_MAX_METRIC);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_build_header
* Returned Values : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static void RIP_build_header(
    uchar_ptr   buf,        /* [OUT] the rip header */
    uint_8      cmd,        /* [IN] the commande */
    uint_8      version     /* [IN] the version */
)
{ /* Body */
    RIP_HEADER_PTR  hd = (RIP_HEADER_PTR)buf;
    _mem_zero(buf, sizeof(RIP_HEADER));
    htonc(hd->COMMAND, cmd);
    htonc(hd->VERSION, version);
    htons(hd->MBZ, 0);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_build_rte
* Returned Values : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static void RIP_build_rte(
    uchar_ptr   buf,    /* [OUT] the route entry */
    uint_16     family,
    uint_16     rt_tag,
    _ip_address net_addr,
    _ip_address net_mask,
    _ip_address next_hop,
    uint_32     metric,
    uint_32     rip_vers
)
{ /* Body */
    RIP_ENTRY_PTR   rte = (RIP_ENTRY_PTR)buf;
    if (rip_vers == RIP_V2){
        htons(rte->RT_TAG, rt_tag);
        htonl(rte->NETMASK, net_mask);
        htonl(rte->NEXTHOP, next_hop);
    }else{
        _mem_zero(buf, sizeof(RIP_ENTRY));
    }
    htons(rte->FAMILY, family);
    htonl(rte->NETADDR, net_addr);
    htonl(rte->METRIC, metric);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_send_pkt
* Returned Values :
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 RIP_send_pkt(
   IP_IF_PTR   ipif,   /* [IN] the outgoing interface */
   _ip_address ipdst,  /* [IN] the destination ip */
   uint_16     portdst,/* [IN] the destination port */
   uchar_ptr   data,   /* [IN] the data to send */
   uint_32     len /* [IN] the length of data to send */
)
{ /* Body */
   RTCSPCB_PTR         pcb;
   RIP_CFG_STRUCT_PTR  ripcfg = RTCS_getcfg(RIP);
   RIP_HEADER_PTR  hd = (RIP_HEADER_PTR)data;
   uint_32         err;
   _ip_address     ipsrc = IP_get_ipif_addr(ipif);

   if (ipsrc == INADDR_ANY) {
       _mem_free(data);
       return RTCS_OK;
   } /* Endif */

   /* Allocate a PCB */
   pcb = RTCSPCB_alloc_send();
   if (pcb == NULL) {
       _mem_free(data);
       return RTCSERR_PCB_ALLOC;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(pcb);

   /* add my data in the pcb */
   err = RTCSPCB_append_fragment_autofree(pcb, len, data);
   if (err) {
       _mem_free(data);
       RTCSLOG_PCB_FREE(pcb, err);
       RTCSPCB_free(pcb);
       return err;
   } /* Endif */

   RTCSLOG_PCB_WRITE(pcb, RTCS_LOGCTRL_PORT(IPPORT_RIP), ntohc(hd->VERSION));

   /* dont advertize in limited broadcast but in ip broadcast */
   if (ipdst == INADDR_BROADCAST){
       _ip_address  netmask;

       ipdst = IP_get_ipif_addr(ipif);        /* Get IP address of interface */
       IP_get_netmask(ipif, ipdst, &netmask); /* Get netmask */

       ipdst = ipdst | ~netmask;
   } /* Endif */

   /* send it */
   /* use a flag to prevent the broadcast/multicast packet from
   ** looping back to us.
   */
   return UDP_send_internal(ripcfg->UCB, ipsrc, ipdst, portdst, pcb, RTCS_MSG_NOLOOP);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_is_valid_rte
* Returned Values :
* Comments        : rfc2453.3.9.2
*
*END*-----------------------------------------------------------------*/

static uint_32 RIP_is_valid_rte(
    RIP_ENTRY_PTR   rte
)
{ /* Body */
    uint_32 metric = ntohl(rte->METRIC);
    _ip_address netaddr = ntohl(rte->NETADDR);
    if (ntohs(rte->FAMILY) != RIP_AF_INET) return 0;
    if (metric > RIP_MAX_METRIC || metric < RIP_MIN_METRIC) return 0;
    if (IN_MULTICAST(netaddr) || IN_EXPERIMENTAL(netaddr)) return 0;
    if (IN_LOOPBACK(netaddr))  return 0;

    {
      _ip_address   nexthop = ntohl(rte->NEXTHOP);
      
      if (nexthop!= 0) {
         if (IP_is_local(NULL, nexthop)) {
            return 0;
         }
      }
    }

    return 1;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_is_valid_resp
* Returned Values :
* Comments        : rfc2453.3.9.2
*
*END*-----------------------------------------------------------------*/

static uint_32 RIP_is_valid_resp(
      RTCSPCB_PTR    pcb          /* [IN/OUT] incoming packet */
)
{ /* Body */
   uint_32 ipsrc = IP_source(pcb);
   uint_16 srcport = UDP_source(pcb);
   /*
   ** check the source port is IPPORT_ROUTE, just a weak security.
   ** On BSD-like network stack, accessing to the ports < 1024
   ** requires privileges.
   */
   if (srcport != IPPORT_RIP) return FALSE;
   /*
   ** if the ipsrc is NOT on a directly connected network which
   ** received the packet, it is not valid
   */
   if (IP_is_direct(pcb->IFSRC,ipsrc) == FALSE) return FALSE;
   /*
   ** If this packet is one of mine, it is not valid.
   ** It can happen in case of more than one interfaces are
   ** directly connected to the same network.
   */
   if (IP_is_local(NULL,ipsrc) == TRUE) return FALSE;
   return 1;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_send_resp_ipif
* Returned Values : boolean
* Comments        :
*
*END*-----------------------------------------------------------------*/

struct RIP_send_resp_ipif_struct  {
   IP_IF_PTR      ipif;
    _ip_address   ipdest;
    uint_32       rip_vers;
    uint_32       onlyChangeF;
    uchar_ptr     pkt;
    uchar_ptr     curs;
    uint_16       portdst;
};

static boolean RIP_send_resp_ipif_test
   (
      _ip_address node_ip,
      _ip_address node_mask,
      pointer     node_data,
      pointer     data
   )
{ /* Body */
   struct RIP_send_resp_ipif_struct _PTR_ testdata = data;
   IP_ROUTE_PTR                           route = node_data;
   IP_ROUTE_INDIRECT_PTR                  gate, end;
   _ip_address                            nexthop, iplist[2];
   uint_32                                metric, rtype, vcount;
   boolean                                complete = FALSE;
   pointer                                rstart[3];

   /* If this node has no entries, return */
   if (!route) {
      return FALSE;
   } /* Endif */

   rstart[0] = route->DIRECT;
   rstart[1] = route->VIRTUAL;
   rstart[2] = route->INDIRECT;

   /*
   ** If there are no connected networks, return. If the node_mask is other than
   ** 255.255.255.255 there is a connected network. There can be a virutal
   ** network even when all the netmask bits are set if the node_ip is different
   ** than the VIRTUAL ADDRESS (like in the case of PPP)
   */
   if (!~node_mask) {
      if (!(route->VIRTUAL && node_ip != route->VIRTUAL->ADDRESS)) {
         return FALSE;
      } /* Endif */

      /* Only check the VIRTUAL routes */
      rstart[0] = NULL;
      rstart[2] = NULL;
   } /* Endif */

   for (rtype = 0, gate = NULL; rtype <= 2 && !gate; rtype++) {
      gate = rstart[rtype];
   } /* Endfor */

   if (!gate) {
      return FALSE;
   } /* Endif */

   end = gate;
   iplist[1] = node_ip;
   do {
      do {
         vcount = 2;
         do {
            /* Don't gather information about tunnels */
            if (gate == rstart[1] && !route->VIRTUAL->ADDRESS) {
               break;
            } /* Endif */

            if (vcount == 1) {
               iplist[0] = route->VIRTUAL->ADDRESS;
            } /* Endif */

            /* allocate the packet memory */
            if (!testdata->pkt)  {
               testdata->pkt = testdata->curs = RTCS_mem_alloc_system(RIP_MAX_PKTLEN);
               if (!testdata->pkt) return RTCSERR_OUT_OF_MEMORY;

               /* build the header */
               RIP_build_header(testdata->curs, RIPCMD_RESPONSE, testdata->rip_vers);
               testdata->curs += sizeof(RIP_HEADER);
            } /* Endif */

            if (testdata->onlyChangeF && !gate->RIP.CHANGED_F) {
               break;
            } /* Endif */

            /* dont advertize the icmp redirect route */
            if (gate->FLAGS & RTF_REDIRECT)  {
               break;
            } /* Endif */

            metric = gate->RIP.METRIC;

#if SPLIT_HORIZON_WITHOUT_POISONNED_REVERSE
            /* split horizon. rfc2453.3.4.3.
            ** DONT advertize a route on the interface on
            ** which you received it.
            */
            if (gate->RIP.IPIFSRC == testdata->ipif)  {
               break;
            } /* Endif */
#else
            /* split horizon with poisoned reverse. rfc2453.3.4.3
            ** DO advertize a route on the interface on
            ** which you received it BUT with a infinit metric.
            */
            if (gate->RIP.IPIFSRC == testdata->ipif)  {
                metric = RIP_MAX_METRIC;
            } /* Endif */
#endif
            /* the outgoing nexthop. rfc2453.4.4. */
            nexthop = 0;
            if (IP_is_direct(testdata->ipif, gate->GATEWAY)) {
               if (end == route->INDIRECT) {
                  nexthop = gate->GATEWAY;
               } /* Endif */
            } /* Endif */


            /* build the route entry */
            RIP_build_rte(testdata->curs, RIP_AF_INET, gate->RIP.RT_TAG
                    , iplist[vcount - 1], node_mask
                    , nexthop, metric, testdata->rip_vers);

            /* check if it is valid before sending it */
            if (!RIP_is_valid_rte((RIP_ENTRY_PTR)testdata->curs)) {
               break;
            } /* Endif */


            testdata->curs += sizeof(RIP_ENTRY);

            /* if the packet is full, send it and come back later */
            if (testdata->curs-testdata->pkt >= sizeof(RIP_HEADER)+RIP_MAX_PKTLEN) {
               RIP_send_pkt(testdata->ipif, testdata->ipdest, testdata->portdst,
                  testdata->pkt, testdata->curs-testdata->pkt);
               /* pkt is freed, reset pointer to NULL */
               testdata->pkt = NULL;
            } /* Endif */
            vcount--;
         } while (end && end == (pointer)route->VIRTUAL && vcount);
         gate = gate->NEXT;
      } while (gate != end);

      for (gate = NULL; rtype <= 2 && !gate; rtype++) {
         gate = rstart[rtype];
      } /* Endfor */
      end = gate;
   } while(gate);
   return FALSE;
} /* Endbody */

static uint_32 RIP_send_resp_ipif (
    IP_IF_PTR   ipif,    /* [IN] the destination interface  */
    _ip_address ipdest,  /* [IN] the destination ip address */
    uint_16     portdst, /* [IN] the destination port */
    uint_32     rip_vers,/* [IN] the rip version */
    uint_32     onlyChangeF /* [IN] TRUE if only the changed routes are sent */
)
{ /* Body */
   IP_CFG_STRUCT_PTR                IP_cfg_ptr = RTCS_getcfg(IP);
   struct RIP_send_resp_ipif_struct testdata;

   testdata.ipif = ipif;
   testdata.ipdest = ipdest;
   testdata.portdst = portdst;
   testdata.onlyChangeF = onlyChangeF;
   testdata.rip_vers = rip_vers;
   testdata.pkt = NULL;
   testdata.curs = NULL;

   IPRADIX_walk(&IP_cfg_ptr->ROUTE_ROOT.NODE, RIP_send_resp_ipif_test, &testdata);

   /* if the packet is empty, dont send it */
   if (testdata.curs-testdata.pkt == sizeof(RIP_HEADER)){
       _mem_free(testdata.pkt);
   } else  {
      /* send the packet */
      RIP_send_pkt(ipif, ipdest, portdst, testdata.pkt,
         testdata.curs - testdata.pkt);
   } /* Endif */

   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_send_resp
* Returned Values : boolean
* Comments        :
*
*END*-----------------------------------------------------------------*/
struct RIP_send_resp_struct  {
   IP_IF_PTR   localhost;
   uint_32     onlyChangeF;
   _ip_address ipdest;
   uint_32     rip_vers;
   uint_32     nb_try;
   uint_32     nb_fail;
};

static boolean RIP_send_resp_test
   (
      _ip_address node_ip,
      _ip_address node_mask,
      pointer     node_data,
      pointer     data
   )
{ /* Body */
   IP_ROUTE_PTR                        route = node_data;
   struct RIP_send_resp_struct _PTR_   testdata = data;
   uint_32                             error;

   if (route && route->DIRECT) {
      /* Don't advertise on local interface */
      if (route->DIRECT->DESTIF != testdata->localhost)  {
         testdata->nb_try++;
         error = RIP_send_resp_ipif(route->DIRECT->DESTIF, testdata->ipdest,
            IPPORT_RIP, testdata->rip_vers, testdata->onlyChangeF);
         if (error != RTCS_OK)  {
            testdata->nb_fail++;
         } /* Endif */
      } /* Endif */
   } /* Endif */

   if (route && route->VIRTUAL && route->VIRTUAL->ADDRESS) {
      /* Don't advertise on local interface */
      if (route->VIRTUAL->DESTIF != testdata->localhost)  {
         testdata->nb_try++;
         error = RIP_send_resp_ipif(route->VIRTUAL->DESTIF, testdata->ipdest,
            IPPORT_RIP, testdata->rip_vers, testdata->onlyChangeF);
         if (error != RTCS_OK)  {
            testdata->nb_fail++;
         } /* Endif */
      } /* Endif */
   } /* Endif */

   return FALSE;
} /* Endbody */

static boolean RIP_send_resp_test2
   (
      _ip_address node_ip,
      _ip_address node_mask,
      pointer     node_data,
      pointer     data
   )
{ /* Body */
   IP_ROUTE_PTR                        route = node_data;
   IP_ROUTE_INDIRECT_PTR               indirect;
   pointer                             p[3];
   uint_32                             i;

   if (!route) {
      return FALSE;
   } /* Endif */

   p[0] = route->DIRECT;
   p[1] = route->INDIRECT;
   p[2] = route->VIRTUAL;

   for (i = 0; i <= 2; i++) {
      if (!p[i]) {
         continue;
      } /* Endif */

      indirect = p[i];
      do {
         indirect->RIP.CHANGED_F = FALSE;
         indirect = indirect->NEXT;
      } while(indirect != p[i]);
   } /* Endfor */

   return FALSE;
} /* Endbody */

static uint_32 RIP_send_resp(
    uint_32 rip_vers,   /* [IN] the rip version */
    uint_32 onlyChangeF /* [IN] TRUE if only the changed routes are sent */
)
{ /* Body */
   RIP_CFG_STRUCT_PTR           ripcfg = RTCS_getcfg(RIP);
   IP_CFG_STRUCT_PTR            IP_cfg_ptr = RTCS_getcfg(IP);
   struct RIP_send_resp_struct  testdata;

   testdata.nb_try = 0;
   testdata.nb_fail = 0;
   testdata.localhost = RTCS_IF_LOCALHOST_PRV;
   testdata.rip_vers = rip_vers;
   testdata.onlyChangeF = onlyChangeF;

   if (rip_vers == RIP_V1) testdata.ipdest = INADDR_BROADCAST;
   else                    testdata.ipdest = INADDR_RIP_GROUP;

   IPRADIX_walk(&IP_cfg_ptr->ROUTE_ROOT.NODE, RIP_send_resp_test, &testdata);

   /* test if the routes has been all advertized (nbFail==0 && nbTry > 0)*/
   if (testdata.nb_fail)        return (uint_32)RTCS_ERROR;
   if (testdata.nb_try == 0)    return RTCS_OK;

   /* clear the flags only if required */
   if (ripcfg->RT_CHANGED_F == FALSE) return RTCS_OK;

   /* update the flag */
   ripcfg->RT_CHANGED_F = FALSE;

   /* clear the flag in all changed route entries */
   IPRADIX_walk(&IP_cfg_ptr->ROUTE_ROOT.NODE, RIP_send_resp_test2, NULL);

   return RTCS_OK;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_process_inreq
* Returned Values : void
* Comments        : check only the header
*
*END*-----------------------------------------------------------------*/

static uint_32 RIP_process_inreq(
      RTCSPCB_PTR    pcb          /* [IN/OUT] incoming packet */
)
{ /* Body */
   RIP_HEADER_PTR  hd = (RIP_HEADER_PTR)RTCSPCB_DATA(pcb);
   RIP_ENTRY_PTR   rte = (RIP_ENTRY_PTR)((uchar_ptr)hd
                   + sizeof(RIP_HEADER));
   uint_32     pktlen = RTCSPCB_SIZE(pcb);
   uint_32     err = RTCS_OK;

   /* handle the general query. rfc2453.3.9.1 */
   if (pktlen == sizeof(RIP_ENTRY) + sizeof(RIP_HEADER)
         && ntohs(rte->FAMILY) == 0
         && ntohl(rte->METRIC) == RIP_MAX_METRIC){
      _ip_address ipsrc = IP_source(pcb);
      uint_16     srcport = UDP_source(pcb);
      uint_8      version = ntohc(hd->VERSION);
      return RIP_send_resp_ipif (pcb->IFSRC,ipsrc,srcport,version,0);
   }
/* WORK: here handle particular routes request */

   return err;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_expire_gc
* Returned Values : boolean
* Comments        : rfc2453.3.8
*
*END*-----------------------------------------------------------------*/

static boolean RIP_expire_gc(
      TCPIP_EVENT_PTR   event      /* [IN/OUT] the resend event */
)
{ /* Body */
    IP_ROUTE_INDIRECT_PTR     gate = event->PRIVATE;

    if (!gate->IS_DIRECT) {
       ROUTE_remove(gate);
    } /* Endif */

    return FALSE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_expire_rte
* Returned Values : boolean
* Comments        : rfc2453.3.8
*
*END*-----------------------------------------------------------------*/

static boolean RIP_expire_rte(
      TCPIP_EVENT_PTR   event      /* [IN/OUT] the resend event */
)
{ /* Body */
    RIP_CFG_STRUCT_PTR     ripcfg = RTCS_getcfg(RIP);
    IP_ROUTE_INDIRECT_PTR  gate = event->PRIVATE;

    /* update the route entry */
    gate->FLAGS &= ~RTF_UP;
    gate->RIP.METRIC = RIP_MAX_METRIC;
    ripcfg->RT_CHANGED_F = gate->RIP.CHANGED_F = TRUE;

    RIP_trig_upd();

    /* start the garbage collector timer. */
    gate->RIP.TIMEOUT.TIME    = RIP_TIMEOUT_GC;
    gate->RIP.TIMEOUT.EVENT   = RIP_expire_gc;
    return TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_init_timer
* Returned Values :
* Comments        : rfc2453.3.9.2
*
*END*-----------------------------------------------------------------*/

static void RIP_init_timer(
    IP_ROUTE_INDIRECT_PTR     gate,
    uint_32                   metric
)
{ /* Body */

    if (metric < RIP_MAX_METRIC){  /* init the timeout */
        gate->RIP.TIMEOUT.TIME    = RIP_TIMEOUT_DEL;
        gate->RIP.TIMEOUT.EVENT   = RIP_expire_rte;
    }else{  /* init the garbage collection */
        gate->RIP.TIMEOUT.TIME    = RIP_TIMEOUT_GC;
        gate->RIP.TIMEOUT.EVENT   = RIP_expire_gc;
    }
    gate->RIP.TIMEOUT.PRIVATE = gate;
    TCPIP_Event_add(&gate->RIP.TIMEOUT);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_adopt_rt
* Returned Values :
* Comments        : rfc2453.3.9.2
*
*END*-----------------------------------------------------------------*/

static void RIP_adopt_rt(
    RTCSPCB_PTR            pcb,          /* [IN] incoming packet */
    IP_ROUTE_INDIRECT_PTR  gate,
    RIP_ENTRY_PTR          rte,
    uint_32                metric,
    _ip_address _PTR_      network_ptr,
    _ip_address _PTR_      netmask_ptr
)
{ /* Body */
   RIP_HEADER_PTR  hd = (RIP_HEADER_PTR)RTCSPCB_DATA(pcb);
   uint_16     rip_vers = ntohc(hd->VERSION);
   _ip_address ipsrc = IP_source(pcb);

   /* init the common part */
   *network_ptr = ntohl(rte->NETADDR);
   gate->GATEWAY = ipsrc;
   if (rip_vers == RIP_V2){
      _ip_address nexthop = ntohl(rte->NEXTHOP);
      *netmask_ptr = ntohl(rte->NETMASK);
      /* incoming nexthop. rfc2453.4.4 */
      if (nexthop && IP_is_direct(pcb->IFSRC, nexthop))
         gate->GATEWAY = nexthop;
   }else{
      *netmask_ptr = IN_DEFAULT_NET(*network_ptr);
   }

   /* if the metric is >= than the RIP_MAX_METRIC, the ourte is down */
   if (metric < RIP_MAX_METRIC) {
      gate->FLAGS |= RTF_UP;
   } else {
      gate->FLAGS &= ~RTF_UP;
   } /* Endif */

   /* init the RIP part */
   gate->RIP.METRIC = metric;
   gate->RIP.RT_TAG = ntohs(rte->RT_TAG);

   gate->RIP.CHANGED_F = TRUE;

   gate->RIP.IPIFSRC = pcb->IFSRC;

   RIP_init_timer(gate, gate->RIP.METRIC);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_update_rt
* Returned Values :
* Comments        : rfc2453.3.9.2
*
*END*-----------------------------------------------------------------*/

static void RIP_update_rt(
    IP_ROUTE_INDIRECT_PTR     gate,
    RTCSPCB_PTR               pcb,          /* [IN] incoming packet */
    RIP_ENTRY_PTR             rte
)
{ /* Body */
   RIP_CFG_STRUCT_PTR  ripcfg = RTCS_getcfg(RIP);
   uint_32         nmetric= RIP_cpu_metric(pcb,ntohl(rte->METRIC));
   _ip_address     ipsrc = IP_source(pcb);


   /* dont modify a static route */
   if (gate->FLAGS & RTF_STATIC)
      return;

   /*
   ** if the source isnt "authoritative" (i.e. not the current
   ** gateway) and have a bigger metric, ignore the route.
   */
   if (ipsrc != gate->GATEWAY && nmetric > gate->RIP.METRIC)
      return;

   /*
   ** if the source isnt "authoritative" (i.e. not the current
   ** gateway) and have a equal metric and will "soon" timed out,
   ** update the route entry.
   */
   if (ipsrc != gate->GATEWAY && nmetric == gate->RIP.METRIC){
      uint_32 timeout = TCPIP_Event_expire(&gate->RIP.TIMEOUT);
      /* if the both metrics are infinite, dont update the route. */
      if (nmetric >= RIP_MAX_METRIC)     return;
      if (timeout > RIP_ALMOST_EXPIRED_TIME) return;
   }

   /* cancel the timeout/gc timer */
   TCPIP_Event_cancel(&gate->RIP.TIMEOUT);

   /* If the sources and the metrics are equal, just reinit the timer. */
   if (ipsrc == gate->GATEWAY && gate->RIP.METRIC == nmetric){
      if (nmetric < RIP_MAX_METRIC){
         RIP_init_timer(gate, gate->RIP.METRIC);
      }
      return;
   }

   /*
   ** Reinit the route and the timer. ipsrc no longer needed, just used
   ** to fill all of the function parameters
   */
   RIP_adopt_rt(pcb, gate, rte, nmetric, &ipsrc, &ipsrc);

   /* flag it as changed */
   ripcfg->RT_CHANGED_F = TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_create_rt
* Returned Values :
* Comments        : rfc2453.3.9.2
*
*END*-----------------------------------------------------------------*/

static void RIP_create_rt(
    RTCSPCB_PTR    pcb,          /* [IN] incoming packet */
    RIP_ENTRY_PTR   rte,
    uint_8      rip_vers
)
{ /* Body */
    uint_32                nmetric= RIP_cpu_metric(pcb,ntohl(rte->METRIC));
    RIP_CFG_STRUCT_PTR     ripcfg = RTCS_getcfg(RIP);
    IP_CFG_STRUCT_PTR      IP_cfg_ptr = RTCS_getcfg(IP);
    IP_ROUTE_INDIRECT_PTR  gate;
    _ip_address            network, netmask;

    gate = RTCS_part_alloc(IP_cfg_ptr->GATE_PARTID);
    if (!gate)  return;
    _mem_zero(gate, sizeof(*gate));

    /* init the route */
    RIP_adopt_rt(pcb, gate, rte, nmetric, &network, &netmask);

    /* insert it in the table */
    ROUTE_insert(gate, network, netmask);

    /* flag it as changed */
    ripcfg->RT_CHANGED_F = TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_process_inresp
* Returned Values :
* Comments        : rfc2453.3.9.2
*
*END*-----------------------------------------------------------------*/

static void RIP_process_inresp(
      RTCSPCB_PTR    pcb          /* [IN/OUT] incoming packet */
)
{ /* Body */
    RIP_CFG_STRUCT_PTR     ripcfg = RTCS_getcfg(RIP);
    uchar_ptr              pkt = RTCSPCB_DATA(pcb);
    RIP_HEADER_PTR         hd = (RIP_HEADER_PTR)RTCSPCB_DATA(pcb);
    RIP_ENTRY_PTR          rte = (RIP_ENTRY_PTR)(pkt + sizeof(RIP_HEADER));
    uint_32                pktlen = RTCSPCB_SIZE(pcb);
    uint_16                rip_vers = ntohc(hd->VERSION);
    IP_ROUTE_INDIRECT_PTR  gate;
    _ip_address            netaddr, netmask;

    if (pktlen < sizeof(RIP_HEADER)) {
        return;
    } /* Endif */

    pktlen -= sizeof(RIP_HEADER);

    if (RIP_is_valid_resp(pcb) == FALSE){
        return;
    } /* Endif */

    for (; pktlen >= sizeof(RIP_ENTRY); pktlen -= sizeof(RIP_ENTRY), rte++){
        if (!RIP_is_valid_rte(rte))
            continue;
        netaddr = ntohl(rte->NETADDR);
        if (rip_vers == RIP_V2) netmask = ntohl(rte->NETMASK);
        else            netmask = IN_DEFAULT_NET(netaddr);
        gate = ROUTE_get(netaddr, netmask);
        if (!gate) RIP_create_rt(pcb, rte, rip_vers);
        else        RIP_update_rt(gate, pcb, rte);
    }
    /* if this incoming response has changed routes, do a triggered update*/
    if (ripcfg->RT_CHANGED_F)
        RIP_trig_upd();

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_service
* Returned Values : void
* Comments        : Process incoming RIP packets.  Called from UDP
*
*END*-----------------------------------------------------------------*/

void RIP_service(
      RTCSPCB_PTR    pcb,        /* [IN/OUT] incoming packet */
      UCB_STRUCT_PTR ucb_ptr     /* [IN]     target UCB      */
)
{ /* Body */
   RIP_HEADER_PTR  hd = (RIP_HEADER_PTR)RTCSPCB_DATA(pcb);


   /* check the version number */
   if (ntohc(hd->VERSION) != RIP_V1 && ntohc(hd->VERSION) != RIP_V2) {
      RTCSLOG_PCB_FREE(pcb, RTCS_OK);
      RTCSPCB_free(pcb);
   } /* Endif */

   RTCSLOG_PCB_READ(pcb, RTCS_LOGCTRL_PORT(IPPORT_RIP), ntohc(hd->VERSION));

   switch (ntohc(hd->COMMAND)) {
   case RIPCMD_REQUEST:
      RIP_process_inreq(pcb);
      break;
   case RIPCMD_RESPONSE:
      RIP_process_inresp(pcb);
      break;
   default:
      break;
   } /* Endswitch */

   RTCSLOG_PCB_FREE(pcb, RTCS_OK);
   RTCSPCB_free(pcb);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_send_req_ipif
* Returned Values : boolean
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 RIP_send_req_ipif (
    IP_IF_PTR   ipif
)
{ /* Body */
    uchar_ptr   pkt;
    uint_32     err;
    _ip_address ipdest;

    /* allocate the packet memory */
    pkt = RTCS_mem_alloc_system(RIP_MAX_PKTLEN);
    if (!pkt) return RTCSERR_OUT_OF_MEMORY;

    /* build the header */
    RIP_build_header(pkt, RIPCMD_REQUEST, ADV_VERSION);

    /* build handle the general query. rfc2453.3.9.1 */
    RIP_build_rte(pkt+sizeof(RIP_HEADER), 0, DEFAULT_ROUTE_TAG, INADDR_ANY
                , INADDR_ANY,INADDR_ANY, RIP_MAX_METRIC
                , ADV_VERSION);

    if (ADV_VERSION == RIP_V1) ipdest = INADDR_BROADCAST;
    else                       ipdest = INADDR_RIP_GROUP;

    /* send the packet */
    err = RIP_send_pkt(ipif, ipdest, IPPORT_RIP, pkt
                , sizeof(RIP_HEADER) + sizeof(RIP_ENTRY));
    if (err != RTCS_OK){
        return err;
    }
    return RTCS_OK;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_send_req
* Returned Values : boolean
* Comments        :
*
*END*-----------------------------------------------------------------*/
struct RIP_send_req_struct  {
   IP_IF_PTR   localhost;
};

static boolean RIP_send_req_test
   (
      _ip_address node_ip,
      _ip_address node_mask,
      pointer     node_data,
      pointer     data
   )
{ /* Body */
   IP_ROUTE_PTR                     route = node_data;
   struct RIP_send_req_struct _PTR_ testdata = data;

   if (route && route->DIRECT && !~node_mask) {
      /* Don't advertise on local interface */
      if (route->DIRECT->DESTIF != testdata->localhost)  {
         RIP_send_req_ipif(route->DIRECT->DESTIF);
      } /* Endif */
   } else if (route && route->VIRTUAL && route->VIRTUAL->ADDRESS) {
      /* Don't advertise on local interface */
      if (route->VIRTUAL->DESTIF != testdata->localhost)  {
         RIP_send_req_ipif(route->VIRTUAL->DESTIF);
      } /* Endif */
      
   } /* Endif */

   return FALSE;
} /* Endbody */

static uint_32 RIP_send_req(
    void
)
{ /* Body */
   IP_CFG_STRUCT_PTR            IP_cfg_ptr = RTCS_getcfg(IP);
   struct RIP_send_req_struct   testdata;

   testdata.localhost = RTCS_IF_LOCALHOST_PRV;
   IPRADIX_walk(&IP_cfg_ptr->ROUTE_ROOT.NODE, RIP_send_req_test, &testdata);

   return RTCS_OK;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_expire_periodic
* Returned Values : boolean
* Comments        :
*
*END*-----------------------------------------------------------------*/

static boolean RIP_expire_periodic(
      TCPIP_EVENT_PTR   event      /* [IN/OUT] the resend event */
)
{ /* Body */

    RIP_send_resp(ADV_VERSION, FALSE);

    event->TIME = RND_RANGE(RIP_TIME_MIN_PERIODIC,
                    RIP_TIME_MAX_PERIODIC);
    return TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_expire_trig_upd
* Returned Values : boolean
* Comments        :
*
*END*-----------------------------------------------------------------*/

static boolean RIP_expire_trig_upd(
      TCPIP_EVENT_PTR   event      /* [IN/OUT] the resend event */
)
{ /* Body */
    RIP_CFG_STRUCT_PTR       ripcfg = event->PRIVATE;

    /* if no route has been during the timer run, just leave */
    if (ripcfg->RT_CHANGED_F == FALSE) {
        /* update the internal flag to know if the timer is running or not */
        ripcfg->TIMER_TRIG_UPD.PRIVATE = NULL;
        return FALSE;
    }

    /* send the changed routes */
    RIP_send_resp(ADV_VERSION, TRUE);

    /* restart the timer */
    event->TIME = RND_RANGE(RIP_TIME_MIN_TRIG_UPD, RIP_TIME_MAX_TRIG_UPD);
    return TRUE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_trig_udp
* Returned Values : boolean
* Comments        :
*
*END*-----------------------------------------------------------------*/

static void RIP_trig_upd(
    void
)
{ /* Body */
    RIP_CFG_STRUCT_PTR     ripcfg = RTCS_getcfg(RIP);

    if (RTCS_getcfg(RIP) == NULL) return;

    /* if the timer is currently running, dont send the data */
    if (ripcfg->TIMER_TRIG_UPD.PRIVATE)
        return;

    /* send the changed routes */
    RIP_send_resp(ADV_VERSION, TRUE);

    /* start the timer */
    ripcfg->TIMER_TRIG_UPD.TIME = RND_RANGE(RIP_TIME_MIN_TRIG_UPD,
                        RIP_TIME_MAX_TRIG_UPD);
    ripcfg->TIMER_TRIG_UPD.EVENT = RIP_expire_trig_upd;
    ripcfg->TIMER_TRIG_UPD.PRIVATE = ripcfg;
    TCPIP_Event_add(&ripcfg->TIMER_TRIG_UPD);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RIP_new_bindif
*  Parameters     :
*  Comments       :
*                 : - can be called before RIP_init
*
*END*-----------------------------------------------------------------*/

void RIP_new_bindif (
    IP_ROUTE_DIRECT_PTR bindif /* [IN] the outgoing binded interface */
)
{ /* Body */
    RIP_CFG_STRUCT_PTR  cfg = RTCS_getcfg(RIP);
    IP_CFG_STRUCT_PTR   IP_cfg_ptr = RTCS_getcfg(IP);
    ip_mreq         mreq;

    if (!cfg) {
       return;
    } /* Endif */

    /* do not init the routing on the local interface */
    if (bindif->NETIF == RTCS_IF_LOCALHOST_PRV) {
       return;
    } /* Endif */

    /* init the multicast group */
    mreq.imr_multiaddr.s_addr = INADDR_RIP_GROUP;
    mreq.imr_interface.s_addr = bindif->ADDRESS;
    IGMP_join_group(&cfg->UCB->MCB_PTR, &mreq, &cfg->UCB->IGMP_LEAVEALL);

    if (bindif->NETIF) {
       RIP_send_req_ipif(bindif->NETIF);
    } else {
       RIP_send_req_ipif(bindif->DESTIF);
    } /* Endif */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_init_static_rt
* Returned Values : void
* Comments        : - called to init the RIP part of a static route
*                     created by ipif_add()
*                 : - can be called before RIP_init
*
*END*-----------------------------------------------------------------*/

void RIP_init_static_rt
(
    IP_ROUTE_INDIRECT_PTR gate,   /* [IN] the route to init */
    uint_16 metric                /* [IN] metric of this route. [0,65535] */
)
{ /* Body */
    RIP_CFG_STRUCT_PTR  ripcfg = RTCS_getcfg(RIP);

    _mem_zero(&gate->RIP, sizeof(gate->RIP));
    gate->RIP.METRIC = metric / 4096;
    /* sanity check */
    if (gate->RIP.METRIC < RIP_MIN_METRIC) {
        gate->RIP.METRIC = RIP_MIN_METRIC;
    } /* Endif */
    if (gate->RIP.METRIC > RIP_MAX_METRIC) {
        gate->RIP.METRIC = RIP_MAX_METRIC;
    } /* Endif */
    /* no src interface for a static route */
    gate->RIP.IPIFSRC = NULL;
    gate->RIP.CHANGED_F = TRUE;
    if (ripcfg){
        ripcfg->RT_CHANGED_F = TRUE;
        RIP_trig_upd();
    }
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RIP_remove_rt
* Returned Values : void
* Comments        : - called to close the RIP part of a route
*                     created by ROUTE_remove()
*
*END*-----------------------------------------------------------------*/

void RIP_remove_rt(IP_ROUTE_INDIRECT_PTR gate)
{ /* Body */
    /* cancel the timeout/gc timer */
    TCPIP_Event_cancel(&gate->RIP.TIMEOUT);
    _mem_zero(&gate->RIP, sizeof(gate->RIP));
} /* Endbody */

#endif
/* EOF */
