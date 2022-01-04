#ifndef __udp_h__
#define __udp_h__
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
* $FileName: udp.h$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the User Datagram Protocol definitions.
*   For more details, refer to RFC768.
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/

/*
** UDP Open Options
*/
#define UDPOPT_CHKSUM_RX   0x0001      /* Option to bypass UDP checksum on Rx */
#define UDPOPT_CHKSUM_TX   0x0002      /* Option to bypass UDP checksum on Tx */
#define UDPOPT_NOBLOCK     0x0004      /* Option to send non-blocking         */


/***************************************
**
** Type definitions
**
*/


/*
** UDP Channel Block for listeners
*/
struct ucb_parm;
struct MCB;
typedef struct ucb_struct {

   struct ucb_struct _PTR_        NEXT;
   uint_32                 SOCKET;

   /* local UDP port */
   uint_16                 PORT;
   uint_16                 PCOUNT;

   void (_CODE_PTR_ SERVICE)(RTCSPCB_PTR, struct ucb_struct _PTR_);

   /*
   ** Queue of incoming packets
   */
   RTCSPCB_PTR    PHEAD;
   RTCSPCB_PTR    PTAIL;

   /*
   ** Queue of waiting receive requests
   */
   struct ucb_parm _PTR_   RHEAD;
   struct ucb_parm _PTR_   RTAIL;

   /* Checksum bypass on reception */
   boolean        BYPASS_RX;

   /* Checksum bypass on transmission */
   boolean        BYPASS_TX;

   /* Non-blocking mode */
   boolean        NONBLOCK_RX;
   boolean        NONBLOCK_TX;

   /* local IP address, usually INADDR_ANY */
   _ip_address    IPADDR;
   in6_addr       IPV6ADDR;  
   uint_32        IF_SCOPE_ID;
    
   /* list of joined multicast groups */
   struct mc_member _PTR_        MCB_PTR;
   uint_32 (_CODE_PTR_     IGMP_LEAVEALL)(struct mc_member _PTR_ _PTR_);

   /*
   ** Determines if a connection failure keeps the ucb's local IP,
   ** or if it resets it to INADDR_ANY
   */
   boolean        KEEP_IPADDR;

   /* Remote IP address and port when connected. (0 when not connected) */
   _ip_address    REMOTE_HOST;
   in6_addr       REMOTE_HOST6;  
   uint_16        REMOTE_PORT;
   uint_16        RESERVED0;

} UCB_STRUCT, _PTR_ UCB_STRUCT_PTR;

/*
** UDP parameters
*/
typedef struct ucb_parm {
   TCPIP_PARM              COMMON;
   struct ucb_parm _PTR_   NEXT;
   UCB_STRUCT_PTR          ucb;
   void (_CODE_PTR_        udpservice)(RTCSPCB_PTR, UCB_STRUCT_PTR);
   _ip_address             ipaddress;
#if RTCSCFG_ENABLE_IP6    
   in6_addr                ipv6address; 
   uint_32                 if_scope_id; 
#endif   
   uint_16                 udpport;
   uint_16                 udpflags;
   pointer                 udpptr;
   uint_32                 udpword;
} UDP_PARM, _PTR_ UDP_PARM_PTR;


/***************************************
**
** Prototypes
**
*/

extern uint_32 UDP_init
(
   void
);

extern void UDP_open     (UDP_PARM_PTR);
extern void UDP_bind     (UDP_PARM_PTR);

#if RTCSCFG_ENABLE_IP6
    extern void UDP_bind6    (UDP_PARM_PTR);
#endif

extern void UDP_connect  (UDP_PARM_PTR);

#if RTCSCFG_ENABLE_IP6
    extern void UDP_connect6 (UDP_PARM_PTR);
#endif

extern void UDP_close    (UDP_PARM_PTR);
extern void UDP_send     (UDP_PARM_PTR);
extern void UDP_receive  (UDP_PARM_PTR);

#if RTCSCFG_ENABLE_IP6
extern void UDP_receive6 (UDP_PARM_PTR);
#endif

extern void UDP_getopt   (UDP_PARM_PTR);
extern void UDP_setopt   (UDP_PARM_PTR);

extern uint_32 UDP_openbind_internal
(
   uint_16              localport,
   void (_CODE_PTR_     service)(RTCSPCB_PTR, UCB_STRUCT_PTR),
   UCB_STRUCT_PTR _PTR_ ucb
);

extern uint_32 UDP_close_internal
(
   UCB_STRUCT_PTR       ucb
);

#if RTCSCFG_ENABLE_IP6
    extern uint_32 UDP_send_internal6
    (
        UCB_STRUCT_PTR       ucb,        /* [IN] UDP layer context      */
        in6_addr             srcaddr,    /* [IN] source IPv6 address      */
        in6_addr             destaddr,   /* [IN] destination IPv6 address */
        uint_16              destport,   /* [IN] destination UDP port   */
        RTCSPCB_PTR          pcb_ptr,    /* [IN] packet to send         */
        uint_32              flags       /* [IN] optional flags         */
    );
#endif


extern uint_32 UDP_send_internal
(
   UCB_STRUCT_PTR       ucb,        /* [IN] UDP layer context      */
   _ip_address          srcaddr,    /* [IN] source IP address      */
   _ip_address          destaddr,   /* [IN] destination IP address */
   uint_16              destport,   /* [IN] destination UDP port   */
   RTCSPCB_PTR          pcb_ptr,    /* [IN] packet to send         */
   uint_32              flags       /* [IN] optional flags         */
);

extern uint_32 UDP_send_IF
(
   UCB_STRUCT_PTR,
   pointer,
   uint_16,
   RTCSPCB_PTR
);


#if RTCSCFG_ENABLE_IP6
    extern void UDP_service6        /* Use function like service for UDP v6*/
    (
        RTCSPCB_PTR,
        pointer
    );
#endif    

#if RTCSCFG_ENABLE_IP4
    extern void UDP_service         /* Use function like service for UDP v4*/
    (
        RTCSPCB_PTR,
        pointer
    );
#endif

    extern void UDP_service_common     /* This function called from UDP_service or UDP_service6 */ 
    (                               /* with parameter of INET FAMILY */
        RTCSPCB_PTR,
        pointer,
        uint_16                     /* INET FAMILY parameter */
    );

extern void UDP_process
(
   RTCSPCB_PTR,
   UCB_STRUCT_PTR
);

#if RTCSCFG_ENABLE_IP6
    extern void UDP_process6
    (
        RTCSPCB_PTR,
        UCB_STRUCT_PTR
    );
#endif    

#endif
/* EOF */
