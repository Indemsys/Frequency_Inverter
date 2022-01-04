#ifndef __ip_prv_h__
#define __ip_prv_h__
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
* $FileName: ip_prv.h$
* $Version : 3.8.12.0$
* $Date    : Oct-9-2012$
*
* Comments:
*
*   Internal definitions for the Internet Protocol layer.
*
*END************************************************************************/

 
    #include "ip6_prv.h"
    #include "ip6_if.h"     


/***************************************
**
** Constants
**
*/

/*
** Fragment field bits
*/
#define IP_FRAG_DF               0x4000   /* If true, do not fragment datagram */
#define IP_FRAG_MF               0x2000   /* If true, this is not last fragment */
#define IP_FRAG_MASK             0x1FFF   /* for offset (/8) of frag in dgram */

#define IP_FRAG_SHIFT            3
#define IP_FRAG_MIN              (1<<IP_FRAG_SHIFT)

/*
** IP option kinds                        length (dec) incl. opt+len bytes
*/
#define IPOPT_COPIED_MASK        0x80     /* set if should be copied on fragm. */
#define IPOPT_CLASS_MASK         0x60
#define IPOPT_CLASS_CONTROL      0x00
#define IPOPT_CLASS_DEBUG_MEASUR 0x40
#define IPOPT_SECURITY           0x02     /*  11                        */
#define IPOPT_LOOSE_ROUTE        0x03     /* var                        */
#define IPOPT_TIMESTAMP          0x44     /* var                        */
#define IPOPT_RECORD_ROUTE       0x07     /* var                        */
#define IPOPT_STREAM_ID          0x08     /*   4 (must not be used )    */
#define IPOPT_STRICT_ROUTE       0x09     /* var                        */

/* Time to live of a datagram awaiting reassembly (no relation to the IP TTL) */
#define IPREASM_TTL              60000       /* 60 secs */


/*
** Options for IP_route
*/

/*
** When we send a packet, first we check all directly connected networks
** for the destination.  If the destination is not directly connected, we
** scan the gateway table for a match.  If we find a match, we once again
** scan all directly connected networks, this time for the gateway.
**
** However, if the gateway fails this test, we do NOT recursively scan the
** gateway table for another gateway to reach the gateway.  IP_route()
** internally uses IPROUTEOPT_GATE to control the level of recursion.
*/
#define IPROUTEOPT_GATE       0x40

/*
** IPROUTEOPT_NOVIRTUAL instructs IP_route() to skip the virtual route
** check when routing a packet.  It's used by IPIP_send() to avoid
** tunneling loops.
*/
#define IPROUTEOPT_NOVIRTUAL  0x80

/*
** When we forward a packet, sometimes we want to make sure it doesn't
** go out the interface it was received on.  This occurs with PPP --
** we never forward a PPP packet back to the peer, because the peer has
** already seen the packet.  (If we did, directed broadcasts on a PPP
** link would bounce between the peers until the TTL expired.)
**
** IP_service() sets IPROUTEOPT_RECVIF in the call to IP_route() when
** it wants to indicate that the received PCB has already been seen
** by all hosts on pcb->IFSRC.  As a result, if IP_route() decides that
** the packet should go out pcb->IFSRC, it will return RTCS_OK without
** sending it (because the next hop has already seen it).
*/
#define IPROUTEOPT_RECVIF     RTCS_MSG_NOLOOP


/***************************************
**
** Code macros
**
*/

/*
** For IP_send[_IF]
*/
#define IPPROTO_GET(i)  ( (i)        & 0xFF)
#define IPTTL_GET(i)    (((i) >>  8) & 0xFF)
#define IPTOS_GET(i)    (((i) >> 16) & 0xFF)
#define IPDFRAG_GET(i)  (((i) >> 24) & 0x01)

#if RTCSCFG_ENABLE_IP_STATS
#define IF_IP_STATS_ENABLED(x) x
#else
#define IF_IP_STATS_ENABLED(x)
#endif


/***************************************
**
** Type definitions
**
*/

/*
** IP packet header
*/
typedef struct IP_HEADER {

   uchar    VERSLEN[1];    /* hi-nybble=Version, lo-nybble=header len/4 */
   uchar    TOS[1];        /* Type of service (see TOS_... #define's) */
   uchar    LENGTH[2];     /* Length of packet (header+data) in bytes */

   uchar    ID[2];         /* Packet identification */
   uchar    FRAGMENT[2];   /* Fragment offset & flags */

   uchar    TTL[1];        /* Time to live, in secs or hops */
   uchar    PROTOCOL[1];   /* Protocol */
   uchar    CHECKSUM[2];   /* IP_checksum */

   uchar    SOURCE[4];     /* sender of packet */
   uchar    DEST[4];       /* destination of packet */

} IP_HEADER, _PTR_ IP_HEADER_PTR;

/*
** IP v6 packet header
*/
typedef struct ip_v6_header {

   uchar    VERSPRIO[1];    /* hi-nybble=Version, lo-nybble=priority */
   uchar    FLOW_LABEL[3];        /* Flow Label  */
   uchar    LENGTH[2];      /* Length of the rest of the packet following the
                              IPv6 header, in octets */
   uchar    NEXT_HEADER[1]; /* Identifies the protocol immediately following
                               the IPv6 header */
   uchar    HOP_LIMIT[1];   /* Number of hops allowed for the packet */
   uchar    SOURCE[16];     /* sender of packet */
   uchar    DEST[16];       /* destination of packet */

} IP_V6_HEADER, _PTR_ IP_V6_HEADER_PTR;

typedef struct ip_v6_min_option_header {
   uchar NEXT_HEADER[1]; /* Identifies the option header immediately following
                            this option. */
   uchar HDR_EXT_LEN[1]; /* Length of the option, excluding the first octet. */
} IP_V6_MIN_OPTION_HEADER, _PTR_ IP_V6_MIN_OPTION_HEADER_PTR;

#define BIGGEST_IP_HEADER  (15*4)
#define IPH_LEN(iph)       ((ntohc((iph)->VERSLEN) & 0xF) << 2)

#if RTCSCFG_ENABLE_IPIF_STATS
#define IF_IPIF_STATS_ENABLED(x) x
#else
#define IF_IPIF_STATS_ENABLED(x)
#endif


/*
** The IP interface
**
** This structure contains enough information to allow IP to
** send packets through a packet driver.
*/
typedef struct ip_if {
#if RTCSCFG_ENABLE_IPIF_STATS
   IPIF_STATS        STATS;
#endif
   pointer           HANDLE;
   RTCS_IF_STRUCT    DEVICE;
   uint_32           MTU;
   pointer           ARP;

   /* fields for BOOTP/DHCP */
   void (_CODE_PTR_  BOOTFN)(RTCSPCB_PTR);
   pointer           BOOT;

   /* fields for ARP/BOOTP/DHCP */
   uint_32           DEV_TYPE;
   uint_32           DEV_ADDRLEN;
   uchar             DEV_ADDR[16];


#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
   /* fields for IGMP */
   struct mc_member _PTR_     IGMP_MEMBER;
   uint_32 (_CODE_PTR_  IGMP_UNBIND)(struct ip_if _PTR_, _ip_address);
#ifdef IGMP_V2
   boolean              IGMP_V1_ROUTER_FLAG;
   uint_32              IGMP_V1_ROUTER_TIMEOUT;
#endif
#endif

#if RTCSCFG_ENABLE_IPSEC
   /* fields for IPsec */
   uint_32           DEFAULT_IPSEC_POLICY;
#endif
#if RTCSCFG_ENABLE_OSPF
   /* fields for Virata OSPF */
   pointer           IFO_ID;
#endif
#if RTCSCFG_ENABLE_SNMP && RTCSCFG_ENABLE_IP4
   /* fields for SNMP */
   uint_32           SNMP_IF_TYPE;
#endif

   /* Calculates MTU. Needs DATA pointer from IP_ROUTE_VIRTUAL struct */
   uint_32 (_CODE_PTR_  MTU_FN)(pointer);
   
   
#if RTCSCFG_ENABLE_IP6 //FSL AB TBD 
    IP6_IF_STRUCT  IP6_IF;     
#endif //FSL AB
   
   
} IP_IF, _PTR_ IP_IF_PTR;

/* the different types of interface. IP_IF.TYPE. (from ospf rfc2328.9) */
#define IP_IFT_POINT2POINT    0x01
#define IP_IFT_BROADCAST      0x02
#define IP_IFT_NBMA           0x03
#define IP_IFT_POINT2MULTI    0x04
#define IP_IFT_VIRTUALLINK    0x05

#if RTCSCFG_ENABLE_RIP && RTCSCFG_ENABLE_IP4 
#include "rip_prv.h" /* included here because it requires IP_IF_PTR */
#endif

/*
** The IP routing table entry
*/

typedef struct ip_route_direct {
   struct ip_route_direct _PTR_  NEXT;
   _ip_address                   ADDRESS;
   IP_IF_PTR                     NETIF;
#if RTCSCFG_ENABLE_RIP && RTCSCFG_ENABLE_IP4 
   RIP_INFO                      RIP;
#endif
   uint_32                       FLAGS;
   IP_IF_PTR                     DESTIF;
} IP_ROUTE_DIRECT, _PTR_ IP_ROUTE_DIRECT_PTR;

typedef struct ip_route_virtual {
   struct ip_route_virtual _PTR_ NEXT;       /* --\                          */
   _ip_address                   ADDRESS;    /* ---\  All of these fields    */
   IP_IF_PTR                     IS_DIRECT;  /* ----\ have the same location */
#if RTCSCFG_ENABLE_RIP && RTCSCFG_ENABLE_IP4 
   RIP_INFO                      RIP;        /* ----/ as in IP_ROUTE_DIRECT. */
#endif
   uint_32                       FLAGS;      /* ---/  This must not change   */
   IP_IF_PTR                     DESTIF;     /* --/                          */
   _ip_address                   SOURCE_NET;
   _ip_address                   SOURCE_MASK;
   pointer                       DATA;
} IP_ROUTE_VIRTUAL, _PTR_ IP_ROUTE_VIRTUAL_PTR;

typedef struct ip_route_indirect {
   struct ip_route_indirect _PTR_   NEXT;        /* Aligned with IP_ROUTE_DIRECT.NEXT    */
   _ip_address                      GATEWAY;     /* Aligned with IP_ROUTE_DIRECT.ADDRESS */
   pointer                          IS_DIRECT;   /* Aligned with IP_ROUTE_DIRECT.NETIF   */
#if RTCSCFG_ENABLE_RIP && RTCSCFG_ENABLE_IP4 
   RIP_INFO                         RIP;         /* Aligned with IP_ROUTE_DIRECT.RIP     */
#endif
   uint_32                          FLAGS;       /* Aligned with IP_ROUTE_DIRECT.FLAGS   */
   /* Start CR 1133 */
   uint_16                          METRIC;
   /* End CR */
} IP_ROUTE_INDIRECT, _PTR_ IP_ROUTE_INDIRECT_PTR;

/*
** This struct relies on the fact that all the different route types start
** start with a NEXT pointer
*/
typedef struct ip_route_prv {
   IPRADIX_NODE              NODE;
   IP_ROUTE_DIRECT_PTR       ROUTETYPE[3];
} IP_ROUTE_PRV, _PTR_ IP_ROUTE_PRV_PTR;

typedef struct ip_route {
   IPRADIX_NODE            NODE;
   IP_ROUTE_DIRECT_PTR     DIRECT;
   IP_ROUTE_INDIRECT_PTR   INDIRECT;
   IP_ROUTE_VIRTUAL_PTR    VIRTUAL;
} IP_ROUTE, _PTR_ IP_ROUTE_PTR;


/* IP_ROUTE_INDIRECT.FLAGS = RTF_* */
#define RTF_UP          0x0001      /* !UP => unreachable */
#define RTF_STATIC      0x0002      /* created by IPIF_(bind|gate) */
#define RTF_REDIRECT    0x0004      /* created for a ICMP redirect */

/*
** IP routing extensions
*/
typedef struct ip_route_fn {
   struct ip_route_fn _PTR_ NEXT;
   void (_CODE_PTR_ INIT_IF)(IP_ROUTE_DIRECT_PTR);
   void (_CODE_PTR_ INIT_RT)(IP_ROUTE_INDIRECT_PTR, uint_16);
   void (_CODE_PTR_ REM_RT)(IP_ROUTE_INDIRECT_PTR);
} IP_ROUTE_FN, _PTR_ IP_ROUTE_FN_PTR;

typedef uint_32 (_CODE_PTR_ IPSEC_FN_PTR)(pointer, pointer, RTCSPCB_PTR _PTR_, _ip_address, _ip_address);

/*
** IP Configuration.  This information is persistent for the IP layer.
*/
typedef struct ip_cfg_struct {
#if RTCSCFG_ENABLE_IP_STATS
   IP_STATS          STATS;
#endif

   ICB_STRUCT_PTR    ICB_HEAD;      /* Head of open ICBs */
   uint_16           NEXT_ID;       /* next packet identifier */
   uint_16           DEFAULT_TTL;   /* default TTL */
   _rtcs_part        ROUTE_PARTID;  /* the partition descriptor of IP_ROUTEs */
#if RTCSCFG_ENABLE_GATEWAYS
   _rtcs_part        GATE_PARTID;   /* the descriptor of IP_ROUTE_INDIRECTS */
#endif
   IP_ROUTE_FN_PTR   ROUTE_FN;      /* function hook for routing protocols */

#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
   _rtcs_part        MCB_PARTID;    /* the partition descriptor of MCBs */
#endif

   _rtcs_part        RADIX_PARTID;
   IP_ROUTE          ROUTE_ROOT;

#if RTCSCFG_ENABLE_IPSEC
   IPSEC_FN_PTR      SEC_SERVICE_FN;
   IPSEC_FN_PTR      SEC_SEND_FN;
   pointer           SEC_FN_ARG;
#endif
#if RTCSCFG_ENABLE_VIRTUAL_ROUTES
   _rtcs_part        VIRTUAL_PARTID;   /* parition for virtual routes */
#endif

} IP_CFG_STRUCT, _PTR_ IP_CFG_STRUCT_PTR;


/***************************************
**
** Prototypes
**
*/

/*
** the IP router
*/

void IP_route_init
(
   IP_ROUTE_PTR   head        /* Pointer to the head of the IPRADIX tree */
);

uint_32 IP_route_add_direct
(
   _ip_address    address,    /* Address or network address of destif   */
   _ip_address    netmask,    /* Mask for the address parameter         */
   IP_IF_PTR      netif,      /* Interface for incomming packets        */
   IP_IF_PTR      destif      /* Interface for outgoing packets         */
);

void IP_route_remove_direct
(
   _ip_address    address,    /* IP address of route to remove    */
   _ip_address    netmask,    /* IP netmask of route to remove    */
   IP_IF_PTR      netif       /* pointer to the route interface   */
);

uint_32 IP_route_add_indirect
(
   _ip_address    address,    /* Gateway address */
   _ip_address    netmask,    /* Network mask    */
   _ip_address    network,    /* Network address */
   uint_32        flag,       /* [IN] RTF_* */
   uint_16        metric      /* [IN] the route metric [0,65535] */
);

void IP_route_remove_indirect
(
   _ip_address    gateway,    /* Gateway address   */
   _ip_address    netmask,    /* Network mask      */
   /* Start CR 1133 */
   _ip_address    network,    /* Network address   */
   uint_32        flag,       /* [IN] RTF_* */
   uint_16        metric      /* [IN] the route metric [0,65535] */
   /* End CR */
);

uint_32 IP_route_add_virtual
(
   _ip_address   address,        /* Destination address              */
   _ip_address   netmask,        /* Mask for the address parameter   */
   _ip_address   source,         /* Interface source address         */
   _ip_address   source_net,     /* Allowed source network           */
   _ip_address   source_mask,    /* Allowed source network mask      */
   IP_IF_PTR     destif,         /* Interface for outgoing packets   */
   pointer       data            /* Route information                */
);

void IP_route_remove_virtual
(
   _ip_address    address,       /* IP address of route to remove    */
   _ip_address    netmask,       /* IP netmask of route to remove    */
   _ip_address    source,        /* IP of interface                  */
   _ip_address    source_net,    /* Allowed source network           */
   _ip_address    source_mask,   /* Allowed source source netmak     */
   IP_IF_PTR      netif          /* pointer to the route interface   */
);

uint_32 IP_complete_send
(
   pointer           ifdest,
         /* [IN] the destination interface */
   RTCSPCB_PTR _PTR_ pcb,
         /* [IN] the packet to send */
   _ip_address       hopsrc,
         /* [IN] the destination interface */
   _ip_address       ipdest,
         /* [IN] the ultimate dest */
   uint_32           protocol
         /* [IN] the transport layer protocol */

);

uint_32 IP_complete_recv
(
   pointer           ifdest,
         /* [IN] the destination interface */
   RTCSPCB_PTR _PTR_ pcb,
         /* [IN] the packet to send */
   _ip_address       hopsrc,
         /* [IN] the destination interface */
   _ip_address       ipdest,
         /* [IN] the ultimate dest */
   uint_32           protocol
         /* [IN] the transport layer protocol */

);

boolean IP_is_local
(
   IP_IF_PTR      iflocal,
         /* [IN] the local interface */
   _ip_address    iplocal
         /* [IN] the IP address to test */
);

boolean IP_is_direct
   (
      IP_IF_PTR      iflocal,
            /* [IN] the local interface */
      _ip_address    iplocal
            /* [IN] the IP address to test */
   );

boolean IP_is_gate
(
   _ip_address    gateway,
         /* [IN] the gateway */
   _ip_address    ipremote
         /* [IN] the IP address to test */
);

boolean IP_get_netmask
(
   _rtcs_if_handle   ihandle,    /* [IN] Interface */
   _ip_address       address,    /* [IN] IP address */
   _ip_address _PTR_ mask_ptr    /* [OUT] netwask for the IP and interface */
) ;

_ip_address IP_get_ipif_addr
(
   IP_IF_PTR      ipif        /* [IN] the local interface */
);

IP_ROUTE_PTR IP_ipif_get_iproute
(
   IP_IF_PTR      ipif        /* [IN] the local interface */
);

uint_32 IP_MTU
(
   _ip_address    iplocal,
         /* [IN] the local IP address */
   _ip_address    ipremote
         /* [IN] the remote IP address to test */
);

IP_IF_PTR IP_find_if
(
   _ip_address    iplocal
         /* [IN] the IP address to test */
);

_ip_address IP_route_find
(
   _ip_address    ipdest,
         /* [IN] the ultimate destination */
   uint_32        flags
         /* [IN] optional flags */
);

uint_32 IP_route_local
(
   RTCSPCB_PTR    pcb,
         /* [IN] the packet to send */
   _ip_address    ipdest
         /* [IN] the ultimate destination */
);

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
);

uint_32 IP_route
(
   RTCSPCB_PTR    pcb,
         /* [IN] the packet to send */
   uint_32        protocol,
         /* [IN] the transport layer protocol */
   _ip_address    hopsrc,
         /* [IN] the destination interface (0 = any) */
   _ip_address    ipsrc,
         /* [IN] the ultimate source */
   _ip_address    ipdest,
         /* [IN] the ultimate destination */
   uint_32        flags
         /* [IN] optional flags */
);

uint_32 IP_send_dgram
(
   IP_IF_PTR      ifdest,     /* [IN] the outgoing interface */
   RTCSPCB_PTR    inpcb,      /* [IN] the packet to send */
   _ip_address    hopsrc,     /* [IN] the hop src */
   _ip_address    hopdest,    /* [IN] the hop dest */
   _ip_address    ipdest,     /* [IN] the ultimate dest */
   uint_32        protocol,   /* [IN] the transport layer protocol */
   pointer        data        /* [IN] routing entry data */
);

/*
** the interface manager
*/

extern IP_IF_PTR            IF_FREE;       /* linked list of free IP_IFs */

extern void IPIF_add         (IPIF_PARM_PTR);
extern void IPIF_bind        (IPIF_PARM_PTR);
extern void IPIF_bind_finish (IPIF_PARM_PTR);
extern void IPIF_bind_ppp    (IPIF_PARM_PTR);
extern void IPIF_gate_add    (IPIF_PARM_PTR);
/* Start CR 1016 */
extern void IPIF_gate_add_redirect(IPIF_PARM_PTR);
/* End CR 1016 */
extern void IPIF_remove      (IPIF_PARM_PTR);
extern void IPIF_unbind      (IPIF_PARM_PTR);
extern void IPIF_unbind_ppp  (IPIF_PARM_PTR);
extern void IPIF_gate_remove (IPIF_PARM_PTR);


/* Local Host Interface.*/
extern IP_IF_PTR   RTCS_IF_LOCALHOST_PRV;

/*
** The local host interface
*/

uint_32 IPLOCAL_open
(
   IP_IF_PTR   if_ptr      /* [IN] the IP interface structure */
);

uint_32 IPLOCAL_close
(
   IP_IF_PTR   if_ptr      /* [IN] the IP interface structure */
);

uint_32 IPLOCAL_send
(
   IP_IF_PTR   if_ptr   ,  /* [IN] the IP interface structure */
   RTCSPCB_PTR pcb      ,  /* [IN] the packet to send */
   _ip_address src      ,  /* [IN] the next-hop source address */
   _ip_address dest     ,  /* [IN] the next-hop destination address */
   pointer     data        /* [IN] unused */
);

uint_32 IPLOCAL_service
(
   RTCSPCB_PTR pcb         /* [IN] the packet to deliver */
);

uint_32 IPLOCAL_init (void);

#if RTCSCFG_ENABLE_IP6
    void IP6LOCAL_open ( IP_IF_PTR if_ptr );
    uint_32 IP6LOCAL_send(  IP_IF_PTR if_ptr     /* [IN] the IP interface structure */,
                            RTCSPCB_PTR rtcs_pcb /* [IN] the packet to send */,
                            in6_addr *src        /* [IN] the next-hop source address */,
                            in6_addr *dest       /* [IN] the next-hop destination address */);
#endif /* RTCSCFG_ENABLE_IP6 */

/*
** the reassembler
*/

/*
** Datagrams awaiting reassembly are stored in sets of 256-byte
** blocks.  We allocate a partition of memory blocks; each block
** can be used in one of three ways:
**
**    1) It can store up to 256 data bytes from the datagram.
**       This is an IPREASM_BLK, called a 'direct block'.
**       This block also has 32-bit bitfield indicating which
**       part(s) of the direct block contain(s) valid data.
**       Each bit represents 8 bytes, so (32 bits) * (8 bytes/bit)
**       = 256 bytes.
**
**    2) It can be used as an array of pointers to IPREASM_BLKs
**       This is an IPREASM_INDIDX, called an 'indirect block'.
**       The indirect block could hold up to 65 direct block
**       pointers (257 in 32-bit wide memory), but we only
**       store 64 (or 256) to simplify the math.  i.e.
**       IPREASM_IND_SIZE could be
**          sizeof(IPREASM_BLK)/sizeof(IPREASM_BLK_PTR),
**       but we only set it to
**          IPREASM_BLK_SIZE/sizeof(IPREASM_BLK_PTR),
**
**    3) It can hold state information about an IP datagram, an
**       IP_DGRAM structure.  In particular, it has the datagram
**       'key' (the IP source, dest, protocol and ID), and as
**       many pointers to indirect blocks as it needs to address
**       a maximum sized datagram (256 direct blocks), i.e. 4
**       (or 1).  However, the IP_DGRAM structure is quite a
**       bit smaller than an IPREASM_BLK, so difference is
**       filled with pointers to direct blocks -- as many as
**       will fit without exceeding the size of an IPREASM_BLK.
**       When allocating blocks for incoming IP datagram fragments,
**       the direct block pointers in the IP_DGRAM are used first,
**       followed by the indirect block pointers.
*/


/* The size of the IP reassembly partition */
#define IPREASM_PART_MAXSIZE     (40*1024)
#define IPREASM_PART_GROW        32          /* 8 KB */
#define IPREASM_PART_MAX         (IPREASM_PART_MAXSIZE / sizeof(IPREASM_BLK))


/* Size of a block in octets */
#define IPREASM_BLK_SIZE         256
#define IPREASM_BLK_NUM(offset)  ((offset) / IPREASM_BLK_SIZE)
#define IPREASM_BLK_OFS(offset)  ((offset) % IPREASM_BLK_SIZE)
#define IPREASM_NB_BLK           (0x10000 / IPREASM_BLK_SIZE)

/* Number of direct block pointers that will fit in an IP_DGRAM */
#define IPREASM_NB_DIRECT        ((sizeof(IPREASM_BLK)-offsetof(IP_DGRAM,DIRECT)) \
                                  / sizeof(IPREASM_BLK_PTR))
#define IPREASM_IS_DIRECT(block) ((block) < IPREASM_NB_DIRECT)

/* Number of direct block pointers in each indirect block */
#define IPREASM_IND_SIZE         (IPREASM_BLK_SIZE / sizeof(IPREASM_BLK_PTR))
#define IPREASM_IND_NUM(block)   (((block)-IPREASM_NB_DIRECT) / IPREASM_IND_SIZE)
#define IPREASM_IND_OFS(block)   (((block)-IPREASM_NB_DIRECT) % IPREASM_IND_SIZE)
#define IPREASM_NB_IND           (IPREASM_NB_BLK / IPREASM_IND_SIZE)

#define IPREASM_IP_OPTIONS_SIZE     (60) 

typedef struct ipreasm_blk {
   uint_32  BUSYINT; /* the bitfield, 1 bit set = 8 bytes copied */
                     /* the LSB is DATA[0-7] */
                     /* the MSB is DATA[249-256] */
   uchar    DATA[IPREASM_BLK_SIZE];
} IPREASM_BLK, _PTR_ IPREASM_BLK_PTR;

/* the structure of an indirect block */
typedef struct ipreasm_indidx {
   IPREASM_BLK_PTR   DIRECT[IPREASM_IND_SIZE];
} IPREASM_INDIDX, _PTR_ IPREASM_INDIDX_PTR;

typedef struct ip_dgram
{
    /* the "key" of the dgram.  rfc815.7 */
    union
    {
    #if RTCSCFG_ENABLE_IP4        
        struct
        {
            _ip_address IPSRC;
            _ip_address IPDST;
            uint_8      PROTO;
            uint_16     ID;
            /* the IP header of the complete datagram */
            IP_HEADER   IPH;
             /* leave rooms for the ip options. MUST follow the IPH field */
            uchar       IP_OPTIONS[60 - sizeof(IP_HEADER)];
            uchar       ICMP_DATA[8];
        }IP4;
    #endif
    #if RTCSCFG_ENABLE_IP6
        struct
        {
            in6_addr    IPSRC;
            in6_addr    IPDST;
            uint_8      PROTO;
            uint_32     ID;
            /* the IP header of the complete datagram */
            IP6_HEADER  IPH;
            uchar       IP_OPTIONS[60];
            uint_32     IPH_OPT_LENGTH;
        }IP6;
    #endif
    }header;
    uint_16     family;    /* AF_INET6/AF_INET */

    /* move-to-front list of IP_DGRAMs */
    struct ip_dgram _PTR_       NEXT;
    struct ip_dgram _PTR_ _PTR_ PREV;

    /* Information from the PCB with the first fragment */
    uint_32              TYPE;
    pointer              IFSRC;
    RTCS_LINKOPT_STRUCT  LINKOPT;

    uint_32    TOTLEN; /* the data of the complete ip datagram payload */
    uint_32    CURLEN; /* the current data size */
    uint_32    MAXLEN; /* the last fragment seen */

    TCPIP_EVENT TIMER; /* the timer to handle datagram expiry */

    IPREASM_INDIDX_PTR   INDIR[IPREASM_NB_IND];
    IPREASM_BLK_PTR      DIRECT[1];
} IP_DGRAM, _PTR_ IP_DGRAM_PTR;

extern IP_DGRAM_PTR  IPREASM_head;  /* list of all IP_DGRAMs */
extern _rtcs_part    IPREASM_part;  /* the partition id for the buffer */

IPREASM_BLK_PTR IPREASM_blk_get      (IP_DGRAM_PTR, uint_32);
uint_32         IPREASM_blk_write    (IP_DGRAM_PTR, IPREASM_BLK_PTR, uint_32, uchar_ptr, uint_32);
void            IPREASM_blk_read_all (IP_DGRAM_PTR, uchar_ptr, uint_32);
void            IPREASM_blk_del_all  (IP_DGRAM_PTR);
void            IPREASM_del_dgram   (IP_DGRAM_PTR);

uint_32 IP_reasm_init (void);
uint_32 IP_reasm      (RTCSPCB_PTR, RTCSPCB_PTR _PTR_);

#endif
/* EOF */
