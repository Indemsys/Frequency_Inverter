#ifndef __tcp_prv_h__
#define __tcp_prv_h__
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
* $FileName: tcp_prv.h$
* $Version : 3.8.10.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Internal definitions for the Transmission Control Protocol layer.
*
*END************************************************************************/


/************************************************************************/
/* Constants                                                            */
/************************************************************************/

/* bits of 'flags' of TCP_HEADER */
#define DATAOFS_MASK          0xf000
#define DATAOFS_SHIFT         12
#define URG                   0x0020
#define ACK                   0x0010
#define PSH                   0x0008
#define RST                   0x0004
#define SYN                   0x0002
#define FIN                   0x0001
#define TCP_RSRV_FIELD_MASK   0xF03F   /* mask off reserved field */

/* option kinds (following header; see RFC 793) - see also ip.h */
                              /* total length */

#define OPT_PAD           0
#define OPT_NOOP          1
#define OPT_MSS           2   /* 4 ; maximum segment size */
#define OPT_WINSCALE      3   /* 3 ; window scale (RFC 1072); unimplemented */
#define OPT_SACKPERM      4   /* 2 ; sack permit. (RFC 1072); unimplemented */
#define OPT_SACK          5   /* ? ; selective ack (RFC 1072); unimplemented*/
#define OPT_ECHO          6   /* 6 ; echo (RFC 1072); unimplemented */
#define OPT_ECHOREPLY     7   /* 6 ; echo reply (RFC 1072); unimplemented */
#define OPT_ALTCHK       14   /* 3 ; altern. checksum (RFC 1146); unimplem. */
#define OPT_ALTCHKDATA   15   /* ? ; alt chksum data (RFC 1146); unimplem. */

/* Delta of the user buffer size above the ring buffer size at which      */
/* to use the user buffer instead of the ring buffer.  This allows larger */
/* window sizes to be given to the remote host.  See comments in          */
/* TCP_Process_receive.                                                        */

#define TCP_SWITCH_THRESHOLD    1024   /* ring buffers are often around 4k */

/*  2k isn't quite enough, since it isn't even twice as big as the maximum
 *   segment size on an Ethernet (about 1400)
 */
#ifndef TCP_DEFAULT_SBSIZE
#define TCP_DEFAULT_SBSIZE      4380   /* Default TCB send buffer size */
#endif
#ifndef TCP_DEFAULT_RBSIZE
#define TCP_DEFAULT_RBSIZE      4380   /* Default TCB receive buffer size */
#endif

#define TCP_MSS_DEFAULT          536   /* Default Maximum segment size,
                                          per RFC 1122 section 4.2.2.6 */

#define TCP_WINDOW_MAX        0xFFFF   /* Largest possible send window */

/* all times in msecs */
#define TCP_INITIAL_RTO_DEFAULT 3000   /* Retransmission timeout when opening
                                          a connection, default 3 seconds
                                          as per RFC1122 */

#define TCP_RTO_MIN  (TICK_LENGTH*3)   /* Wait at least two ticks before
                                          retransmitting a packet; imposed
                                          by granularity of timer hardware
                                          (if we waited only one tick, the
                                          tick might expire right away...) */


#define TCP_MSL               120000L  /* Maximum Segment Lifetime; the
                                          longest time that a packet can
                                          travel in the Internet (2 min) */
#define TCP_WAITTIMEOUT (2 * TCP_MSL)  /* timeout for TIME_WAIT state, defined
                                          as 2 * MSL (4 min) */
#define TCP_SENDTIMEOUT_MIN   100000L  /* as per RFC1122 4.2.3.5 */
#define TCP_OPENTIMEOUT_MIN   180000L  /* as per RFC1122 4.2.3.5 */


#define TCP_SWSDELAY               80   /* TCP_Delay before sending less-than-full
                                         data packet [...] */

#define TCP_DEFAULT_SECUREDRAFT_0  FALSE /* default choice about wether to invoke behaviour 
                                            of draft-ietf-tcpm-tcpsecure-00.txt */

/*  We don't currently send any IP options (but should be able to);
 *   upper limit for IP header size is 64, so TCP_IPOPTIONS_MAX can be
 *   at most 64 - IP_HEADSIZE = 44
 */
#define TCP_IPOPTIONS_MAX          0
#define TCP_HEADSIZE              20
#define TCP_OPTIONS_MAX            4   /* we only add the MSS option (and
                                          then, only when sending SYN);
                                          upper limit for TCP header size
                                          is 64, so this can be at most
                                          64-TCP_HEADSIZE = 44 */

/* TCB states */
#define LISTEN         0
#define SYN_SENT       1
#define SYN_RECEIVED   2
#define ESTABLISHED    3
#define FINWAIT_1      4
#define FINWAIT_2      5
#define CLOSE_WAIT     6
#define CLOSING        7
#define LAST_ACK       8
#define TIME_WAIT      9
#define CLOSED        10
#define BOUND         11

/* a TCB (in this implem.) is never in CLOSED state "before" being opened; once closed,
a TCB may be deleted, but not reused */

#define UNKNOWNSTATE  12 /* must be greater than any state nb */

/* TCP output processing states */
#define TCP_IDLE     0
#define TCP_PROBE    1
#define TCP_XMIT     2

/************************************************************************/
/* Macros                                                               */
/************************************************************************/

/* Comparisons in modulo 2**32 sequence-number arithmetic: (RFC sec.3.3)
 *  assumes parameters are 32-bit sequence numbers
 *
 * Since you may find modulo comparisons ambiguous:  when comparing
 *  against a 32-bit number x, the 32-bit sequence space is divided in
 *  three with respect to x, that is:
 *
 *    range  x+1 to x+0x7fffffff ,        (said to be "greater than x")
 *    range  x+0x80000000 to x-1 , and    (said to be "less than x")
 *    x itself                            (said to be "equal to x" :-) ).
 *
 * For example, if x is 0xe1112131:
 *
 *    0xe1112132 to 0xffffffff and 0 to 0x61112130 (are "greater than x")
 *    0x61112131 to 0xe1112130                     (are "less than x").
 *
 * All this is perfectly ok since windows are of maximum size 64K which
 *  is many orders of magnitude smaller than 2^31...  (there's a bit more
 *  to this but that's basically it)
 */
#define GT32(a,b)      ((int_32)((a)-(b)) >  0)    /* a >  b */
#define GE32(a,b)      ((int_32)((a)-(b)) >= 0)    /* a >= b */
#define LT32(a,b)      ((int_32)((a)-(b)) <  0)    /* a <  b */
#define LE32(a,b)      ((int_32)((a)-(b)) <= 0)    /* a <= b */

/*  Security and precedence are not implemented; these are macros that
 *   are called where something must be done to implement them.
 */

/* if packet's security/compartment doesn't match
 *  that of tcb, send reset and return
 */
#define CHECK_SECURITY  DEBUGTCP(printf("\nTCP: Check Security");)

/* if packet's security/compartment doesn't match
 *  that of tcb, send reset, delete tcb with ECONNRESET and return
 */
#define CHECK_TIGHT_SECURITY  DEBUGTCP(printf("\nTCP: Check Tight Security");)

/* if packet's precedence is greater than tcb's, and system & user allow it,
 *  set tcb's precedence to that of packet; if not allowed, send reset & return.
 * (do nothing if packet's precedence is equal or lower)
 */
#define CHECK_PRECEDENCE   DEBUGTCP(printf("\nTCP: Check Precedence");)

/* if packet's precedence doesn't exactly match tcb's, send reset & return.
 */
#define CHECK_EXACT_PRECEDENCE   DEBUGTCP(printf("\nTCP: Check Exact Precedence");)

/*  Events which are not "state", and may occur more than once, and thus
 *   should automatically be cleared (from tcb->options) after being
 *   returned to client task.
 */
#define TCPS_REOCCURRING  (TCPS_TREADY | TCPS_TACKED | TCPS_TIMEOUT | \
                           TCPS_SOFTERROR | TCPS_NOWAIT | TCPS_DEFER)

/*  Events which are "state", and are only set once during connection lifetime,
 *   then never cleared.
 */
#define TCPS_SETONCE      (TCPS_STARTED | TCPS_FINACKED | TCPS_FINRECV |\
                           TCPS_FINDONE | TCPS_FINTOSEND)

/*  Useful state groups.
 */
#define TCPSTATE_FINSENT(state)  ((state)==FINWAIT_1 || (state)==FINWAIT_2|| \
                                  (state)==CLOSING   || (state)==TIMEWAIT || \
                                  (state)==CLOSED)
#define TCPSTATE_FINACKED(state) ((state)==FINWAIT_2 || (state)==TIMEWAIT || \
                                  (state)==CLOSED)
#define TCPSTATE_FINRECV(state)  ((state)==CLOSEWAIT || (state)==LASTACK  || \
                                  (state)==CLOSING   || (state)==TIMEWAIT || \
                                  (state)==CLOSED)
#define TCPSTATE_TCLOSED(state)  ((state)==FINWAIT_2 || (state)==TIMEWAIT || \
                                  (state)==CLOSED)
#define TCPSTATE_RCLOSED(state)  TCPSTATE_FINRECV(state)
#define TCPSTATE_STARTED(state)  ((state)==ESTABLISHED || \
                                  (state)==FINWAIT_1 || (state)==FINWAIT_2|| \
                                  (state)==CLOSEWAIT || (state)==LASTACK  || \
                                  (state)==CLOSING   || (state)==TIMEWAIT || \
                                  (state)==CLOSED)

#define TCP_DROP     {IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_DISCARDED++); /* Used in TCP_Service_packet*/ \
                      DEBUGTCP(printf("\nTCP_Service_packet: Dropping packet")); \
                      goto tcp_sp_drop; }

#define TCP_PP_DROP  {IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_DISCARDED++); /* Used in TCP_Process_packet */ \
                      DEBUGTCP(printf("\nTCP_Process_packet: Dropping packet")); \
                      goto tcp_pp_drop; }

#define TCP_PO_FAIL(ecode)  { result = (ecode); /* Used in TCP_Process_open */ \
                              DEBUGTCP(printf("\nTCP_Process_open: Failing")); \
                              goto failopen; }

#define TCP_PS_REPLY(ecode) { result = (ecode); /* Used in TCP_Process_send */ \
                              DEBUGTCP(printf("\nTCP_Process_send: Replying (result=%x)",result)); \
                              goto reply_send; }

#define TCB_HALF_OPEN_DROP { \
                              uint_32 tcb_cnt; \
                              for (tcb_cnt = 0; tcb_cnt < tcp_cfg->HALF_OPEN_TCB_COUNT; tcb_cnt++) { \
                                 if (tcb == tcp_cfg->HALF_OPEN_TCB_LIST[tcb_cnt]) { \
                                    tcp_cfg->HALF_OPEN_TCB_COUNT--; \
                                    tcp_cfg->HALF_OPEN_TCB_LIST[tcb_cnt] = \
                                    tcp_cfg->HALF_OPEN_TCB_LIST[tcp_cfg->HALF_OPEN_TCB_COUNT]; \
                                    break; \
                                 } /* Endif */ \
                              } /* Endfor */ \
                           }


/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/*  Header of a TCP packet
 */
typedef struct tcp_header {              /* see RFC sec.3.1 */

   uchar    source_port[2];   /* source TCP port number */
   uchar    dest_port[2];     /* destination TCP port number */
   uchar    seq[4];           /* of 1st data octet or 1st-1 if (flags & SYN)*/
   uchar    ack[4];           /* valid if (flags & ACK) */
   uchar    flags[2];         /* see flags values above */
   uchar    window[2];        /* nb of bytes recipient of packet can send */
   uchar    checksum[2];      /* (see RFC) */
   uchar    urgent[2];        /* offset from seq, valid if (flags & URG) */
                              /* then follow options in 4-byte multiples, */
                              /* then data  */

} TCP_HEADER, _PTR_ TCP_HEADER_PTR; /* 20 bytes */

/************************************************************************/
/* Functions                                                            */
/************************************************************************/

/*******************************/
/* From tcp.c                  */
/*******************************/

extern void TCP_Event
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   int_16                  /* IN     - event that occurred */
);

extern void TCP_Return_open
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   uint_32              ,  /* IN     - reason code for returning */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer constants */
);

extern TCB_STRUCT_PTR TCP_Clone_tcb
(
   TCB_STRUCT_PTR          /* IN = the listening TCB to clone */
);

extern void TCP_Process_packet
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   RTCSPCB_PTR          ,  /* IN/OUT - the incoming packet */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer constants */
);

#if RTCSCFG_ENABLE_IP6 
extern void TCP6_Process_packet
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   RTCSPCB_PTR          ,  /* IN/OUT - the incoming packet */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer constants */
);
#endif




/*******************************/
/* From tcp_buf.c              */
/*******************************/

extern uint_32 TCP_Copy_to_ring
(
            uchar _PTR_ ,  /* IN - source data buffer */
            uint_32     ,  /* IN - size of data */
   register int_32      ,  /* IN - where in ring to put data */
            uchar _PTR_ ,  /* IN - start of ring buffer */
            uint_32        /* IN - total size of ring buffer */
);

extern uint_32 TCP_Copy_from_ring
(
            uchar _PTR_ ,  /* IN - destination buffer */
            uint_32     ,  /* IN - size of data at ringofs */
   register int_32      ,  /* IN - offset within ring buffer */
            uchar _PTR_ ,  /* IN - source ring buffer */
            uint_32        /* IN - ring buffer total size */
);

extern uint_32 TCP_Copy_from_buffer
(
   TCB_STRUCT_PTR tcb,     /* IN - TCP context */
   uchar_ptr      data,    /* IN - destination buffer */
   uint_32_ptr    size_ptr,/* IN/OUT - number of bytes to copy */
   uint_32        offset,  /* IN - offset from start of transmit buffer */
   RTCSPCB_PTR    pcb_ptr  /* IN - PCB used to send the packet */
);

extern uint_32 TCP_Copy_to_Rbuffer
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   uint_32              ,  /* IN     - destination offset from
                                       rcvpos in rcvbuf */
   uchar _PTR_          ,  /* IN     - source data */
   uint_32              ,  /* IN     - size of data */
   uchar _PTR_          ,  /* IN     - receive buffer */
   uint_32              ,  /* IN     - total size of rcv buffer */
   uint_32              ,  /* In     - current position in rcvbuf */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer data */
);

extern uint_32 TCP_Use_ringbuffer
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer data */
);

/*******************************/
/* From tcp_rcv.c              */
/*******************************/

extern void  TCP_Must_send_ack
(
   TCB_STRUCT_PTR       ,     /* IN/OUT - TCP layer context */
   TCP_CFG_STRUCT_PTR         /* IN     - TCP layer data */
);

extern void TCP_Advance_receive_user
(
   TCB_STRUCT_PTR       ,     /* IN/OUT - TCP context */
   int_32                     /* IN     - no. of bytes given to user */
);

extern void TCP_Truncate_receive_chunks
(
   TCB_STRUCT_PTR       ,     /* IN/OUT - TCP context */
   TCP_CFG_STRUCT_PTR   ,     /* IN/OUT - TCP layer data */
   int_32                     /* IN     - length of data to remove */
);

extern void TCP_Advertise_receive_window
(
   TCB_STRUCT_PTR       ,     /* IN/OUT - TCP layer context */
   TCP_CFG_STRUCT_PTR         /* IN/OUT - TCP layer data */
);

extern void TCP_Start_receiving
(
   TCB_STRUCT_PTR       ,     /* IN/OUT - TCP layer context */
   uint_32              ,     /* IN     - initial receive sequence */
   TCP_CFG_STRUCT_PTR         /* IN/OUT - TCP layer data */
);

extern void TCP_Reply_receive
(
   TCB_STRUCT_PTR       ,     /* IN/OUT - TCP context */
   TCP_CFG_STRUCT_PTR   ,     /* IN/OUT - TCP layer data */
   int_32                     /* IN     - error code to return */
);

extern boolean TCP_Test_done_receive
(
   TCB_STRUCT_PTR          tcb,     /* IN/OUT - TCP context */
   uint_32                 status,  /* IN     - error code  */
   TCP_CFG_STRUCT_PTR      tcp_cfg  /* IN/OUT - TCP layer data */
);

extern void TCP_Process_data
(
   TCB_STRUCT_PTR       ,     /* IN/OUT - TCP context */
   TCP_CFG_STRUCT_PTR   ,     /* IN/OUT - TCP layer data */
   TCP_HEADER _PTR_     ,     /* IN     - data within packet */
   RTCSPCB_PTR                /* IN     - the packet */
);

extern boolean  TCP_Setup_receive
(
   TCP_PARM_PTR            ,  /* IN/OUT - the Receive request */
   TCB_STRUCT_PTR          ,  /* IN/OUT - TCP context */
   TCP_CFG_STRUCT_PTR         /* IN/OUT - TCP layer data */
);

/*******************************/
/* From tcp_send.c             */
/*******************************/

extern void TCP_Update_send_window
(
   TCB_STRUCT_PTR    ,     /* IN/OUT - TCP context */
   TCP_HEADER _PTR_        /* IN     - the incoming ACK packet */
);

extern SndClock _PTR_ TCP_Process_RTT_measure
(
   TCB_STRUCT_PTR    ,     /* IN/OUT - TCP context */
   int_32                  /* IN     - time packet was received */
);

#if RTCSCFG_ENABLE_IP6 
extern uint_32 TCP6_Send_IP_packet
(
   RTCSPCB_PTR          pcb,         /* IN/OUT - packet to send */
   in6_addr            *srchost,    /* IN     - IPv6 source address */
   in6_addr            *desthost,   /* IN     - IPv6 dest address */
   boolean              bypass      /* IN     - TRUE to bypass checksum */
);
#endif

extern uint_32 TCP_Send_IP_packet
(
   RTCSPCB_PTR          rtcs_pcb,   /* IN/OUT - packet to send */
   _ip_address          srchost,    /* IN     - source address */
   _ip_address          desthost,   /* IN     - dest address */
   boolean              bypass      /* IN     - TRUE to bypass checksum */
);

extern void TCP_Send_packet
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP layer context */
   uint_32              ,  /* IN     - data sequence number */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer constants */
);

extern void TCP_Send_reply_reset
(
   TCB_STRUCT_PTR       tcb,        /* IN/OUT - TCP context */
   _ip_address          source,     /* IN     - destination for reset */
   TCP_HEADER _PTR_     received,   /* IN     - received TCP packet */
   uint_16              rlen,       /* IN     - length of received packet */
   TCP_CFG_STRUCT_PTR   tcp_cfg,    /* IN/OUT - TCP layer constants */
   boolean              bypass_chk  /* IN     - TRUE to bypass checksum */
);

#if RTCSCFG_ENABLE_IP6
extern void TCP6_Send_reply_reset
(
   TCB_STRUCT_PTR       tcb,        /* IN/OUT - TCP context */
   in6_addr            *source,     /* IN     - IPv6 destination for reset */
   TCP_HEADER _PTR_     received,   /* IN     - received TCP packet */
   uint_16              rlen,       /* IN     - length of received packet */
   TCP_CFG_STRUCT_PTR   tcp_cfg,    /* IN/OUT - TCP layer constants */
   boolean              bypass_chk  /* IN     - TRUE to bypass checksum */
);
#endif

extern void TCP_Send_reset
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer constants */
);

extern void TCP_Send_keepalive
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer constants */
);

extern void TCP_Transmit
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP layer context */
   TCP_CFG_STRUCT_PTR   ,  /* IN/OUT - TCP layer constants */
   boolean              ,  /* IN     - Should reset trans. ptr? */
   boolean                 /* IN     - Guarantee send? */
);

extern void TCP_Start_sending
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP layer context */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer data */
);

extern void TCP_Process_ack
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   TCP_HEADER _PTR_     ,  /* IN     - packet header */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer data */
);

/*******************************/
/* From tcp_clos.c             */
/*******************************/

extern void TCP_Free_TCB
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer data */
);

extern void TCP_Close_TCB
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   int_32               ,  /* IN     - error code to return */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer data */
);

extern void TCP_Process_effective_close
(
   TCB_STRUCT_PTR       ,  /* IN/OUT - TCP context */
   TCP_CFG_STRUCT_PTR      /* IN     - TCP layer data */
);

extern void TCP_Process_release
(
   TCB_STRUCT_PTR       ,  /* IN     - TCP context */
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer data */
);

void TCP_half_open_TCB_close
(
   TCP_CFG_STRUCT_PTR      /* IN/OUT - TCP layer data */
);

#endif

/*******************************/
/* DEBUGGING Macros            */
/*******************************/
#define dprintf(x)  printf x

/*
** Uncomment to enable TCP Debugging

#define DO_TCP_STATE_DEBUG 0
#define DO_TCP_TCB_DEBUG 0

*/

#ifdef DO_TCP_STATE_DEBUG
#define TCP_STATE_DEBUG(x) { printf ("TCPSTATE %s(%d): ", __FUNCTION__, __LINE__); printf x ;}
#else
#define TCP_STATE_DEBUG(x) 
#endif


#ifdef DO_TCP_TCB_DEBUG
#define TCP_TCB_DEBUG(x) { printf ("TCP:TCB %s(%d): ", __FUNCTION__, __LINE__); printf x ;}
#else
#define TCP_TCB_DEBUG(x) 
#endif



/* EOF */
