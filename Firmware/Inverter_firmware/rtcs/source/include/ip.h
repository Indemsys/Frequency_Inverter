#ifndef __ip_h__
#define __ip_h__
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
* $FileName: ip.h$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Definitions for the Internet protocol layer.
*
*END************************************************************************/

#include "rtcstime.h"

/***************************************
**
** Constants
**
*/

/*
** IP datagram sizes
*/

#define IP_HEADSIZE        20       /* sizeof(IP_HEADER)                 */

#define IP_DEFAULT_MTU     576      /* minimum IP datagram size which    */
                                    /* must be supported by all IP hosts */
#define IP_MAX_MTU         0xFFFF   /* size of largest IP datagram       */

/*
** default IP parameters for outgoing packets
*/
#define IP_VERSION         4        /* Version 4               */
#define IP_VERSION_4       4        /* Version 4               */
#define IP_VERSION_6       6        /* Version 6               */
/* Start CR 1899 */
#define IPTTL_DEFAULT      64       /* Time to Live (60 hops)  */
/* End CR 1899 */
#define IPTOS_DEFAULT      0        /* Type of Service         */

#define IP_HEADER_LENGTH_MASK  0x0F

#define IPTTL(ttl)         ((uint_32)(ttl) <<  8)
#define IPTOS(tos)         ((uint_32)(tos) << 16)
#define IPDF(df)           ((uint_32)(df)  << 24)

/*
**
** Ports < IPPORT_RESERVED are reserved for
**  privileged processes (e.g. root).
**
** Ports > IPPORT_USERRESERVED are reserved
**  for servers, not necessarily privileged.
*/
#define IPPORT_RESERVED          1024
#define IPPORT_USERRESERVED      5000


/***************************************
**
** Type definitions
**
*/

/*
** Multicast group members
*/

typedef struct mc_member {
   ip_mreq           IGRP;       /* the group description */
   uint_32           UCOUNT;     /* usage counter */
   struct mc_member _PTR_  NEXT;       /* pointer to the next in the list */

   /* used only by the interface part */
   TCPIP_EVENT TIMER;
   boolean     RUNNING_TIMER; /* true if the timer is running */
   boolean     LAST_REPORTER; /* true if we are the last host to report on this group */
   uint_32     UNSOLICITED_REPORT_COUNTER;   /* the number of unsolicited reports remaining to send */

   /*
   ** it's unnecessary to store a state (i.e. non-member, delaying-member, idle-member)
   ** because non-members don't have a MC_MEMBER, delaying-member is when
   ** the running_timer is true, and idle-member when running_timer is true
   */

} MC_MEMBER, _PTR_ MC_MEMBER_PTR;

/*
** Function for servicing packets
*/

typedef void (_CODE_PTR_ IP_SERVICE_FN)(RTCSPCB_PTR, pointer);

/*
** Internet Channel Block
*/
typedef struct icb_struct {

   struct icb_struct _PTR_  NEXT;          /* next ICB in the chain      */
   uint_32           PROTOCOL;      /* protocol for connection    */

   IP_SERVICE_FN     SERVICE;       /* Owner's service function    */
   pointer           PRIVATE;       /* Owner's configuration block */

} ICB_STRUCT, _PTR_ ICB_STRUCT_PTR;


/***************************************
**
** Prototypes
**
*/

extern uint_32 IP_init
(
   void
);

extern ICB_STRUCT_PTR IP_open
(
   uchar             ,     /* [IN]  Protocol to use */
   IP_SERVICE_FN     ,     /* [IN]  Packet receive function */
   pointer           ,     /* [IN]  Owner's config block */
   uint_32_ptr             /* [OUT] return code */
);

extern uint_32 IP_send
(
   RTCSPCB_PTR       ,     /* [IN] the packet to send */
   uint_32           ,     /* [IN] transport protocol, TTL and TOS */
   _ip_address       ,     /* [IN] the destination interface (0 = any) */
   _ip_address       ,     /* [IN] the ultimate destination */
   uint_32                 /* [IN] optional flags */
);

extern uint_32 IP_send_IF
(
   RTCSPCB_PTR       ,     /* [IN] the packet to send */
   uint_32           ,     /* [IN] transport protocol, TTL and TOS */
   pointer                 /* [IN] the destination interface */
);

extern void IP_service
(
   RTCSPCB_PTR             /* [IN] received packet */
);

extern _ip_address IP_source
(
   RTCSPCB_PTR             /* [IN] packet to find source of */
);

extern _ip_address IP_dest
(
   RTCSPCB_PTR             /* [IN] packet to find destination of */
);

#if RTCSCFG_ENABLE_IP6
    #include "ip6.h"
#endif


#endif
/* EOF */
