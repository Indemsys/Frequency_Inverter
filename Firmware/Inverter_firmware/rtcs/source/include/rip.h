#ifndef __rip_h__
#define __rip_h__
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
* $FileName: rip.h$
* $Version : 3.0.6.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   Definitions for the RIP protocol layer.
*
*END************************************************************************/

#include "ip_prv.h"

#if RTCSCFG_ENABLE_RIP_STATS
#define IF_RIP_STATS_ENABLED(x) x
#else
#define IF_RIP_STATS_ENABLED(x)
#endif


/*
** RIP packet header
*/
typedef struct rip_header {
   uchar    COMMAND[1];
   uchar    VERSION[1];
   uchar    MBZ[2];
} RIP_HEADER, _PTR_ RIP_HEADER_PTR;

/*
** RIP packet entry
*/
typedef struct rip_entry {
   uchar    FAMILY[2];
   uchar    RT_TAG[2];
   uchar    NETADDR[4];
   uchar    NETMASK[4];
   uchar    NEXTHOP[4];
   uchar    METRIC[4];
} RIP_ENTRY, _PTR_ RIP_ENTRY_PTR;

/*
** RIP Configuration.  This information is persistent for the RIP layer.
*/
typedef struct rip_cfg_struct {
#if RTCSCFG_ENABLE_RIP_STATS
   RIP_STATS      STATS;
#endif

   TCPIP_EVENT    TIMER_PERIODIC; /* for periodic advertisement */
   TCPIP_EVENT    TIMER_TRIG_UPD; /* to avoid a excessive traffic with
                                  ** the trigger updates
                                  */

   boolean        RT_CHANGED_F;   /* TRUE if routes have been modified */

   UCB_STRUCT_PTR UCB; /* the struture to send/rcv udp packet */
} RIP_CFG_STRUCT, _PTR_ RIP_CFG_STRUCT_PTR;

/* list of rip commands */
#define RIPCMD_REQUEST          1       /* want info - from suppliers */
#define RIPCMD_RESPONSE         2       /* responding to request */
        /* unofficial commands (ie gated ones) */
#define RIPCMD_TRACEON          3       /* turn tracing on */
#define RIPCMD_TRACEOFF         4       /* turn it off */
#define RIPCMD_POLL             5       /* like request, but anyone answers */
#define RIPCMD_POLLENTRY        6       /* like poll, but for entire entry */

/* rip limits */
#define RIP_MAX_METRIC          16      /* per Xerox NS */
#define RIP_MIN_METRIC          1       /* per Xerox NS */
#define RIP_MAX_PKTLEN          512     /* max broadcast size */

/* rip timer */
#if 1   /* the official values */
# define RIP_TIMEOUT_DEL         (180*1000)  /* time to mark entry invalid */
# define RIP_TIMEOUT_GC          (120*1000)  /* time to garbage collect */
# define RIP_TIME_MIN_TRIG_UPD   (1*1000)    /* rfc2453.3.10.1 */
# define RIP_TIME_MAX_TRIG_UPD   (5*1000)
# define RIP_TIME_MIN_PERIODIC   (25*1000)   /* rfc2453.3.8 */
# define RIP_TIME_MAX_PERIODIC   (35*1000)
#else   /* the debug values */
# define RIP_TIMEOUT_DEL         (18*1000)   /* time to mark entry invalid */
# define RIP_TIMEOUT_GC          (12*1000)   /* time to garbage collect */
# define RIP_TIME_MIN_TRIG_UPD   (1*1000)    /* rfc2453.3.10.1 */
# define RIP_TIME_MAX_TRIG_UPD   (5*1000)
# define RIP_TIME_MIN_PERIODIC   (1*1000)    /* rfc2453.3.8 */
# define RIP_TIME_MAX_PERIODIC   (2*1000)
#endif


/* if the route will timeout in less than RIP_ALMOST_EXPIRED_TIME, it is
** considered as dying and can be replaced by an equal metric route.
** rfc2453.3.9.2
*/
#define RIP_ALMOST_EXPIRED_TIME (RIP_TIMEOUT_DEL/5)

#define RIP_V1  1
#define RIP_V2  2


void RIP_init_static_rt(IP_ROUTE_INDIRECT_PTR gate, uint_16 metric);
void RIP_remove_rt(IP_ROUTE_INDIRECT_PTR gate);
void RIP_new_bindif(IP_ROUTE_DIRECT_PTR bindif);

#endif  /* __rip_h__ */
/* EOF */
