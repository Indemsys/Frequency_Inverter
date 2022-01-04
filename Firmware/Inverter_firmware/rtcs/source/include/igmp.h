#ifndef __igmp_h__
#define __igmp_h__
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
* $FileName: igmp.h$
* $Version : 3.8.8.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the Internet Group Mangement protocol
*   User Datagram Protocol definitions.
*   For more details, refer to RFC1112 and RFC2236.
*
*END************************************************************************/


#if 0
#  define IGMP_V2      /* to define if the host has to support IGMPv2 */
#endif

#if RTCSCFG_ENABLE_IGMP & RTCSCFG_ENABLE_IP4

/***************************************
**
** Constants
**
*/

/* IGMP packet types */
#define IGMPTYPE_QUERY        0x11
#define IGMPTYPE_V1_REPORT    0x12
#define IGMPTYPE_V2_REPORT    0x16
#define IGMPTYPE_LEAVE        0x17

/* IGMP constants */
#define IGMP_V1_ROUTER_TIMEOUT_VALUE     (400*1000)
#define IGMP_V1_QUERY_RESPONSE_INTERVAL  ( 10*1000)   /* default max_resp_time in 1/1000 sec fro IGMPv1 */
#define IGMP_UNSOLICITED_REPORT_INTERVAL ( 10*1000)
#define IGMP_UNSOLICITED_REPORT_COUNT    2      /* number of repetition. if == 2, 3 reports will be sent */


/***************************************
**
** Code macros
**
*/

#if RTCSCFG_ENABLE_IGMP_STATS
#define IF_IGMP_STATS_ENABLED(x) x
#else
#define IF_IGMP_STATS_ENABLED(x)
#endif

/***************************************
**
** Type definitions
**
*/

/*
** IGMP parameters for [gs]etsockopt
*/
typedef struct igmp_parm {
   TCPIP_PARM              COMMON;
   struct igmp_parm _PTR_  NEXT;
   SOCKET_STRUCT_PTR       sock;
   uint_32                 option;
   pointer                 optptr;
   uint_32                 optlen;
} IGMP_PARM, _PTR_ IGMP_PARM_PTR;

/*
** IGMP Configuration.  This information is persistent for the IGMP layer.
*/
typedef struct igmp_cfg_struct {
#if RTCSCFG_ENABLE_IGMP_STATS
   IGMP_STATS     STATS;         /* the statistic for IGMP */
#else
   uchar          Reserved;
#endif
} IGMP_CFG_STRUCT, _PTR_ IGMP_CFG_STRUCT_PTR;

typedef struct igmp_header {
   uchar    TYPE[1];
   uchar    MAX_RESP_TIME[1];
   uchar    CHECKSUM[2];
   uchar    GROUP_ADDRESS[4];
} IGMP_HEADER, _PTR_ IGMP_HEADER_PTR;


/***************************************
**
** Prototypes
**
*/

struct ip_if;

extern uint_32 IGMP_init (void);
extern boolean IGMP_send_report (ip_mreq _PTR_, uint_16);

extern uint_32 IGMP_filter_add (struct ip_if _PTR_, _ip_address);
extern uint_32 IGMP_filter_rm  (struct ip_if _PTR_, _ip_address);

extern MC_MEMBER_PTR _PTR_ IGMP_member_find   (MC_MEMBER_PTR _PTR_, ip_mreq _PTR_);
extern MC_MEMBER_PTR _PTR_ IGMP_member_create (MC_MEMBER_PTR _PTR_, ip_mreq _PTR_);
extern uint_32             IGMP_member_delete (MC_MEMBER_PTR _PTR_);

extern void IGMP_init_timer   (MC_MEMBER_PTR);
extern void IGMP_launch_timer (MC_MEMBER_PTR, uint_32);
extern void IGMP_stop_timer   (MC_MEMBER_PTR);

extern uint_32 IGMP_ipif_add  (struct ip_if _PTR_);
extern uint_32 IGMP_ipif_bind (struct ip_if _PTR_, _ip_address);

extern boolean IGMP_is_member
(
   MC_MEMBER_PTR _PTR_  phead,      /* [IN] head of the MCB list */
   struct ip_if _PTR_   ipif,       /* [IN] the incoming interface */
   _ip_address          multiaddr   /* [IN] the multicast ip */
);

extern uint_32 IGMP_join_group  (MC_MEMBER_PTR _PTR_, ip_mreq _PTR_, uint_32 (_CODE_PTR_ _PTR_)(MC_MEMBER_PTR _PTR_));
extern uint_32 IGMP_leave_group (MC_MEMBER_PTR _PTR_, ip_mreq _PTR_, uint_32 (_CODE_PTR_ _PTR_)(MC_MEMBER_PTR _PTR_));

#endif
#endif
/* EOF */
