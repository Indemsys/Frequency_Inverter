#ifndef __ip6_prv_h__
#define __ip6_prv_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: ip6_prv.h$
* $Version : 3.8.1.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Internal definitions for the Internet Protocol layer.
*
*
*END************************************************************************/

#include "rtcs.h"
#include "ip.h"

#if RTCSCFG_ENABLE_IP6_STATS
#define IF_IP6_STATS_ENABLED(x) x
#else
#define IF_IP6_STATS_ENABLED(x)
#endif


/*********************************************************************
*
* Type definitions
*
*********************************************************************/

/* RFC4484:
 *  Multicast destination addresses have a 4-bit scope field that
 *  controls the propagation of the multicast packet.  The IPv6
 *  addressing architecture defines scope field values for interface-
 *  local (0x1), link-local (0x2), subnet-local (0x3), admin-local (0x4),
 *  site-local (0x5), organization-local (0x8), and global (0xE)
 *  scopes [11].
 */
#define IP6_ADDR_SCOPE_INTFACELOCAL (0x01)
#define IP6_ADDR_SCOPE_LINKLOCAL    (0x02)
#define IP6_ADDR_SCOPE_SITELOCAL    (0x05)
#define IP6_ADDR_SCOPE_ORGLOCAL     (0x08)
#define IP6_ADDR_SCOPE_GLOBAL       (0x0e)


#define IP6_ADDR_MULTICAST_SCOPE(a) ((a)->s6_addr[1] & 0x0f)

/*********************************************************************
* IP packet header
**********************************************************************
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |Version| Traffic Class |           Flow Label                  |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |         Payload Length        |  Next Header  |   Hop Limit   |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |                                                               |
*   +                                                               +
*   |                                                               |
*   +                         Source Address                        +
*   |                                                               |
*   +                                                               +
*   |                                                               |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |                                                               |
*   +                                                               +
*   |                                                               |
*   +                      Destination Address                      +
*   |                                                               |
*   +                                                               +
*   |                                                               |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**********************************************************************/
typedef struct IP6_HEADER
{
   uchar    VERSION_TCLASS[1];	/* 4-bit Internet Protocol version number = 6, 8-bit traffic class field. */
   uchar    TCLASS_FLOWL[1]; 	/* 20-bit flow label. */
   uchar    FLOWL[2];        
   uchar    LENGTH[2];			/* Length of the IPv6
								 * payload, i.e., the rest of the packet following
							     * this IPv6 header, in octets. */
   uchar    NEXT_HEADER[1]; 	/* Identifies the type of header
                        		 * immediately following the IPv6 header.  Uses the
                        		 * same values as the IPv4 Protocol field [RFC-1700
                        		 * et seq.].*/
   uchar    HOP_LIMIT[1];   	/* Decremented by 1 by
                        		 * each node that forwards the packet. The packet
                        		 * is discarded if Hop Limit is decremented to
                        		 * zero. */
   uchar    SOURCE[16];     	/* 128-bit address of the originator of the packet. */
   uchar    DEST[16];       	/* 128-bit address of the intended recipient of the
								 * packet (possibly not the ultimate recipient, if
								 * a Routing header is present). */

} IP6_HEADER, _PTR_ IP6_HEADER_PTR;

#define IP6_HEADER_LEN		    (40) /* IPv6 Header size. */

#define IP6_HEADER_VERSION		0x60
#define IP6_HEADER_VERSION_MASK	0xf0

/******************************************************************
* Extension header types
*******************************************************************/
#define IP6_TYPE_HOP_BY_HOP_OPTIONS                     (0)
#define IP6_TYPE_DESTINATION_OPTIONS                    (60)
#define IP6_TYPE_ROUTING_HEADER                         (43)
#define IP6_TYPE_FRAGMENT_HEADER                        (44)
#define IP6_TYPE_AUTHENTICATION_HEADER                  (51)
#define IP6_TYPE_ENCAPSULATION_SECURITY_PAYLOAD_HEADER  (50)
#define IP6_TYPE_MOBILITY_HEADER                        (135)
#define IP6_TYPE_NO_NEXT_HEADER                         (59)

/***********************************************************************
 * Hop-by-Hop Options Header & Destination Options Header
 *
 * The Hop-by-Hop Options header is used to carry optional information
 * that must be examined by every node along a packet’s delivery path.
 *
 * The Destination Options header is used to carry optional information
 * that need be examined only by a packet’s destination node(s).
 ***********************************************************************
 * RFC 2460 4.3/4.6:
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | Next Header   | Hdr Ext Len   |                               |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 *  |                                                               |
 *  .                                                               .
 *  .                         Options                               .
 *  .                                                               .
 *  |                                                               |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 ***********************************************************************/
typedef struct IP6_OPTIONS_HEADER
{
   uchar    NEXT_HEADER[1];     /* 8-bit selector. Identifies the type of header
                                 * immediately following the Options
                                 * header. */
   uchar    HDR_EXT_LENGTH[1];  /* 8-bit unsigned integer. Length of the Hop-by-
                                 * Hop Options header in 8-octet units, not
                                 * including the first 8 octets. */
   uchar    OPTIONS[6];         /* Variable-length field, of length such that the
                                 * complete Options header is an integer
                                 * multiple of 8 octets long. */
} IP6_OPTIONS_HEADER, _PTR_ IP6_OPTIONS_HEADER_PTR;


/***********************************************************************
 * Options (used in op-by-Hop Options Header & Destination Options Header) 
 ***********************************************************************
 * RFC 2460 4.2:
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - - -
 * | Option Type   | Opt Data Len  | Option Data
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - - -
 ***********************************************************************/
typedef struct IP6_OPTION_HEADER
{
   uchar    TYPE[1];        /* 8-bit identifier of the type of option. */
   uchar    DATA_LENGTH[1]; /* 8-bit unsigned integer. Length of the Option
                             * Data field of this option, in octets. */
} IP6_OPTION_HEADER, _PTR_ IP6_OPTION_HEADER_PTR;

/* RFC 2460 4.2: There are two padding options which are used when necessary to align
 * subsequent options and to pad out the containing header to a multiple
 * of 8 octets in length. */

#define IP6_OPTION_TYPE_PAD1    (0x00)  /* The Pad1 option is used to insert 
                                         * one octet of padding into the Options area of a header.*/
#define IP6_OPTION_TYPE_PADN    (0x01)  /* The PadN option is used to insert two or more octets of padding
                                         * into the Options area of a header. For N octets of padding, the
                                         * Opt Data Len field contains the value N-2, and the Option Data
                                         * consists of N-2 zero-valued octets. */


/* RFC 2460: The Option Type identifiers are internally encoded such that their
 * highest-order two bits specify the action that must be taken if the
 * processing IPv6 node does not recognize the Option Type:*/
#define IP6_OPTION_TYPE_UNRECOGNIZED_MASK           (0xC0)

#define IP6_OPTION_TYPE_UNRECOGNIZED_SKIP           (0x00)  /* 00 - skip over this option and continue processing the header.*/
#define IP6_OPTION_TYPE_UNRECOGNIZED_DISCARD        (0x40)  /* 01 - discard the packet. */
#define IP6_OPTION_TYPE_UNRECOGNIZED_DISCARD_ICMP   (0x80)  /* 10 - discard the packet and, regardless of whether or not the
                                                             * packet’s Destination Address was a multicast address, send an
                                                             * ICMP Parameter Problem, Code 2, message to the packet’s
                                                             * Source Address, pointing to the unrecognized Option Type.*/
#define IP6_OPTION_TYPE_UNRECOGNIZED_DISCARD_UICMP  (0xC0)  /* 11 - discard the packet and, only if the packet’s Destination
                                                             * Address was not a multicast address, send an ICMP Parameter
                                                             * Problem, Code 2, message to the packet’s Source Address,
                                                             * pointing to the unrecognized Option Type.*/

/***********************************************************************
 * Routing Header
 *
 * The Routing header is used by an IPv6 source to list one or more
 * intermediate nodes to be "visited" on the way to a packet’s
 * destination.
 ***********************************************************************
 * RFC 2460 4.4:
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | Next Header   | Hdr Ext Len   | Routing Type  | Segments Left |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                                                               |
 *  .                                                               .
 *  .                       type-specific data                      .
 *  .                                                               .
 *  |                                                               |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 ***********************************************************************/
typedef struct IP6_ROUTING_HEADER
{
    uchar    NEXT_HEADER[1];    /* 8-bit selector. Identifies the type of header
                                 * immediately following the Options
                                 * header. */
    uchar    HDR_EXT_LENGTH[1]; /* 8-bit unsigned integer. Length of the Hop-by-
                                 * Hop Options header in 8-octet units, not
                                 * including the first 8 octets. */
    uchar    ROUTING_TYPE[1];   /* 8-bit identifier of a particular Routing header
                                 * variant.*/ 
    uchar    SEGMENTS_LEFT[1];  /* 8-bit unsigned integer. Number of route
                                 * segments remaining, i.e., number of explicitly
                                 * listed intermediate nodes still to be visited
                                 * before reaching the final destination.*/                                                                
    uchar    DATA[4];           /* Variable-length field, of format determined by
                                 * the Routing Type, and of length such that the
                                 * complete Routing header is an integer multiple
                                 * of 8 octets long. */
} IP6_ROUTING_HEADER, _PTR_ IP6_ROUTING_HEADER_PTR;







/* Time to live of a datagram awaiting reassembly (no relation to the IP TTL) */
/* RFC 2460: If insufficient fragments are received to complete reassembly of a
 * packet within 60 seconds of the reception of the first-arriving
 * fragment of that packet, reassembly of that packet must be
 * abandoned and all the fragments that have been received for that
 * packet must be discarded.*/
#define IP6REASM_TTL              60000       /* 60 secs */

/***********************************************************************
 * Fragment Header
 ***********************************************************************
 * RFC 2460 4.5:
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |  Next Header  |   Reserved    |      Fragment Offset    |Res|M|
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                         Identification                        |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 ***********************************************************************/
typedef struct IP6_FRAGMENT_HEADER
{
   uchar    NEXT_HEADER[1]; /* 8-bit selector.  Identifies the initial header
                             * type of the Fragmentable Part of the original
                             * packet (defined below).  Uses the same values as
                             * the IPv4 Protocol field [RFC-1700 et seq.]. */
   uchar    RESERVED[1]; 	/* Initialized to zero for
                             * transmission; ignored on reception. */
   uchar    OFFSET_MORE[2]; /* @ 13-bit unsigned integer.  The offset, in 8-octet
                             * units, of the data following this header,
                             * relative to the start of the Fragmentable Part
                             * of the original packet.
                             * @ 2-bit reserved field.  Initialized to zero for
                             * transmission; ignored on reception.
                             * @ 1 = more fragments; 0 = last fragment.*/       
   uchar    ID[4];			/* Identification. */
} IP6_FRAGMENT_HEADER, _PTR_ IP6_FRAGMENT_HEADER_PTR;


#define IP6_FRAGMENT_MF             0x1     /* If 1, this is not last fragment */
#define IP6_FRAGMENT_OFFSET_MASK    0xFFF8  /* The offset of frag in dgram */

/***********************************************************************
* IPv6 Configuration.  This information is persistent for the IPv6 layer.
***********************************************************************/
typedef struct ip6_cfg_struct
{
#if RTCSCFG_ENABLE_IP6_STATS
    IP6_STATS         STATS;
#endif
    ICB_STRUCT_PTR    ICB6_HEAD;        /* Head of open ICBs, used by IPv6 */
    uint_32           NEXT_ID6;         /* Next packet identifier */
    uint_16           DEFAULT_HOPLIMIT; /* Default Hop Limit */

} IP6_CFG_STRUCT, _PTR_ IP6_CFG_STRUCT_PTR;

/******************************************************************
* Function Prototypes
*******************************************************************/
struct ip_if; /* Just to avoid header conflicts.*/
uint_32 IP6_send_dgram ( struct ip_if *ifdest, RTCSPCB_PTR inrtcs_pcb, in6_addr *hopsrc, in6_addr *hopdest, in6_addr *ipdest, uint_32 protocol);
void ip6_get_solicited_multicast_addr(in6_addr *ip_addr, in6_addr *solicited_multicast_addr);

uint_32 IP6_reasm_init (void);
uint_32 IP6_reasm      (RTCSPCB_PTR, RTCSPCB_PTR _PTR_);


uint_32 IP6_MTU ( in6_addr *iplocal     /* [IN] the local IP address */,
                  in6_addr *ipremote    /* [IN] the remote IP address to test */ );
in6_addr * IP6_route_find( in6_addr *ipdest   /* [IN] the ultimate destination */,
                                 uint_32 flags      /* [IN] optional flags (Not used - TBD delete it*/); 
int ip6_common_prefix_length(in6_addr *ip_addr_1, in6_addr *ip_addr_2);                                                  
int ip6_addr_scope(in6_addr *ip_addr);
boolean ip6_addr_pefix_cmp(in6_addr *addr_1, in6_addr *addr_2, uint_32 prefix_length);

#endif /* __ip6_prv_h__ */
