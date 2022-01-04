#ifndef __tcp_h__
#define __tcp_h__
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
* $FileName: tcp.h$
* $Version : 3.8.10.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Definitions for the Transmission Control Protocol layer.
*
*END************************************************************************/


/*
** References
**
** [1] "Congestion Avoidance and Control", Van Jacobson, ACM SIGCOMM-88,
**      August 1988 (1988 ACM 0-89791-279-9/88/008/0314)
** [2] "Round Trip Time Estimation" (not full title),
**      P. Karn & C. Partridge, ACM SIGCOMM-87, August 1987
** RFC 793
** RFC 1122
** RFC 896
*/
#include "ip.h"
#include "ticker.h"

/************************************************************************/
/* Constants                                                            */
/************************************************************************/

/*
 *  Useful TCP assigned port numbers (see RFC 1010) (as in /etc/services)
 */
#define TCPPORT_ECHO       7   /* see RFC862; returns what was sent */
#define TCPPORT_DISCARD    9   /* see RFC863; returns nothing */
#define TCPPORT_TELNET    23   /* see RFC854, RFC855, ... */
#define TCPPORT_EXEC     512   /* `exec' service; see man rexecd(8C) */
#define TCPPORT_LOGIN    513   /* `login' service; see man rlogind(8C) */
#define TCPPORT_SHELL    514   /* `cmd' service; see man rshd(8C) */

/*  Options bits for TCB, also used in send and open calls
 */
#define TCPO_ACTIVE    0x0001  /* open/option: actively start connection */
#define TCPO_NONAGLE   0x0002  /* open/option: disable Nagle algorithm */
#define TCPO_NOSWITCH  0x0004  /* open/option: disable using user rcv buffer */
#define TCPO_NOWAIT    0x0010  /* send/rcv: return immediately */
#define TCPO_PUSH      0x0020  /* send/rcv: push data          */
#define TCPO_URGENT    0x0040  /* send/rcv: send bytes as urgent data */
#define TCPO_WAIT      0x0080  /* send/rcv: always block       */
                               /*    (wait for ack of data)    */

/*  Status bits for TCB, also used as event bits in wait and receive calls:
 */
#define TCPS_DUMMY     0x0000  /* Dummy event to TCP_Event */
#define TCPS_STARTED   0x0001  /* set once ESTABLISHED (or in any state in
                                  TCPSTATE_STARTED(state)), never cleared */
#define TCPS_FINACKED  0x0002  /* see TCPSTATE_FINACKED(state) */
#define TCPS_FINRECV   0x0004  /* see TCPSTATE_FINRECV(state) */
#define TCPS_FINDONE   0x0008  /* both FINACKED and FINRECV (not either) */
#define TCPS_FINTOSEND 0x2000  /* Close requested, FIN not necessarily sent */
#define TCPS_TPUSH     0x0010  /* PSH being sent, tcb->sndpush is valid */
#define TCPS_RPUSH     0x0020  /* PSH received, tcb->rcvpush is valid */
#define TCPS_TURGENT   0x0040  /* URG being sent, tcb->sndup is valid */
#define TCPS_RURGENT   0x0080  /* URG received, tcb->rcvup is valid */

/* transients */
#define TCPS_TREADY    0x0100  /* more data can be transmitted; set when send
                                  buffer becomes less than half full, cleared
                                  when send buffer becomes full */
#define TCPS_TACKED    0x0200  /* set when all sent data was acked; cleared if
                                  any more data is sent */
#define TCPS_TIMEOUT   0x0400  /* transmit timeout (transmission not ack'ed)*/
#define TCPS_SOFTERROR 0x0800  /* non-fatal ("soft") errors */
#define TCPS_NOWAIT    0x1000  /* return immediately */
#define TCPS_DEFER     0x4000  /* receive: asynchronous receive into reader's
                                  buffer; or, reader's buffer has been
                                  (asynchronously) filled */

#define TCB_VALID_ID   0x4b454720L
#define TCB_INVALID_ID 0x20544159L

/************************************************************************/
/* Macros                                                               */
/************************************************************************/
#if RTCSCFG_ENABLE_TCP_STATS
#define IF_TCP_STATS_ENABLED(x) x
#else
#define IF_TCP_STATS_ENABLED(x)
#endif

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/*
** TCP options set by upper layer
*/
typedef struct tcp_options_struct  {

   /*
   ** TCP_Transmit buffer size (use 0 to disable TCP buffer i.e. alway block on send,
   ** use -1 to default)
   */
   int_32      tbsize;

   int_32      rbsize;        /* Receive buffer size (use 0 to enforce
                                 that a user receive must be available, use
                                 -1 to default) */

   int_32      utimeout;      /* User timeout (msec) - TCP R1 period */
   int_32      timeout;       /* Connection timeout (msec) - TCP R2 period
                                 (use 0 to default) */
   int_32      rto;           /* Initial retransmission timeout (use 0 to
                                 default) */
   int_32      maxrto;        /* Max. retransmission timeout (use 0 to
                                 default) */
   uint_32     maxrcvwnd;     /* Max. receive window size (use 0 to
                                 default) */

   uchar       RESERVED2;
   uchar       ttl;           /* Time to live (use 0 to default) */
   uchar       tos;           /* Specify Internet type of service (use 0
                                 to default) */
   uchar       precede;       /* Specify precedence (0 to 7) (overrides
                                 precedence set in "tos" field) (use 0xFF
                                 to default) */

   _ip_address lochost;       /* Local host IP address (use IP_ZERO to
                                 default) */
   _ip_address remhost;       /* Remote host to call or wait for (may be
                                 IP_ZERO if can accept from any host) */

  #if RTCSCFG_ENABLE_IP6 
   in6_addr lochost_ipv6;       /* Local host IPv6 address (use in6addr_any to
                                 default) */
   in6_addr remhost_ipv6;       /* Remote host to call or wait for (may be
                                 in6addr_any if can accept from any host) */
  #endif
                                 

   uint_16     locport;       /* Local TCP port - use 0 to have one
                                 assigned */
   uint_16     remport;       /* Remote port to call or wait for (may be
                                 0 if can accept from any host port) */

   int_32      keepalive;     /* "Keepalive" timeout (0 to disable) */

   boolean     active;        /* True when originating connection, false
                                 to listen */
   boolean     nowait;        /* Return requests immediately to upper layer
                                 without waiting for completion */
   boolean     nonagle;       /* Disable the Nagle algorithm */
   boolean     noswrbuf;      /* Disable switching to user receive buffer */

   boolean     bypass_rx_chksum;  /* TRUE to bypass checksum check on Rx */
   boolean     bypass_tx_chksum;  /* TRUE to bypass checksum calc. on Tx */

   int_32     timewaitto;     /* TIME_WAIT timeout (use 0 for 2*MSL) */
   boolean     tcpsecuredraft0; /* draft-ietf-tcpm-tcpsecure-00.txt */
   uint_32 delayackto;   /* delay ack timeout */
} TCP_OPTIONS_STRUCT, _PTR_ TCP_OPTIONS_PTR;

/*
** TCP parameters
*/
typedef struct tcb_parm {
   TCPIP_PARM              COMMON;
   struct tcb_parm _PTR_   NEXT;

   uint_32        OPCODE;        /* Command to TCP */
   uint_32        ERROR_CODE;    /* Error code returned */

   struct tcb_struct _PTR_ TCB_PTR;     /* TCP Context (TCB) */
   TCP_OPTIONS_PTR  OPTIONS_PTR; /* TcpOpen options */

   _ip_address    IP_ADDR;

#if RTCSCFG_ENABLE_IP6 
    in6_addr      IPV6_ADDR;     /* IPv6 address     */
    uint_32       IF_SCOPE_ID;   /* Interface number */
#endif
    uint_16        AF;           /* Inet family      */  

   uint_16        TCP_PORT;
   uint_16        RESERVED1;

   pointer        BUFFER_PTR;    /* Send or receive buffer */
   uint_32        BUFF_LENG;     /* Send or receive buffer length */
   int_32         TIMEOUT;       /* Send or receive timeout */

   uint_16        OPTIONS;       /* Send or receive flags */
   uint_16        STATUS;        /* TCP connection status */

   uint_32        URGENT;        /* Length of urgent data in buffer */
   int_32         SOFTERROR;     /* Last error code in TCP context */

   uint_32        SOCKET;        /* Upper layer socket structure */

} TCP_PARM, _PTR_ TCP_PARM_PTR;

typedef struct tcp_message_struct
{
   MESSAGE_HEADER_STRUCT   HEADER;
   TCP_PARM                TCP_REQUEST;
} TCP_MESSAGE_STRUCT, _PTR_ TCP_MESSAGE_STRUCT_PTR;


/*  Send buffer chain, describes contents of transmit buffer; some parts
 *   of it may be in the TCB's `sndringbuf', other parts may be from the
 *   clients' buffers themselves.
 */
typedef struct SbufNode {

   struct SbufNode _PTR_ next;/* next buffer in this output stream */

   uchar _PTR_          data; /* if use_cbuf is TRUE, this is ptr to client
                                 Send buffer; else (client is FALSE), this is
                                 the offset (by casting to (uint_32)) in the
                                 TCB's `sndringbuf' of a piece of data to be
                                 sent (may wrap around the ring buffer) */

   int_32               size; /* nb of bytes (in client buf or in ring) */

   TCP_PARM_PTR         req;  /* pointer to Send request, or NULL */

   boolean              use_cbuf;
                              /* TRUE if the Send request buffer is
                                 being used to transmit, FALSE if
                                 transmitting from transmit ring buffer */

} SbufNode;

/*  An array of these is in each TCB (sndclks), to record transmit times for
 *   packets (segments)...
 */
typedef struct sndclock {

    uint_32   seq;      /* 1st seq # past last byte transmitted */
    int_32    time;     /* time (INFO(Time)) when packet transmitted */

} SndClock;

/*  These are used for all timers within TCB's; since expired TimeQ's can
 *   be automatically put in lists, the `tcb' field indicates in which TCB
 *   the expired TimeQ's are located.
 */
typedef struct tcbtimeq {

    TimeQ            t;     /* timer (see ticker.h); *must be 1st element**/
    struct tcb_struct _PTR_ tcb;   /* TCB which contains this timer */

} TcbTimeQ;

/*  Describes portions of the receive buffer which have been filled (received)
 */
typedef struct Rchunk {

    struct Rchunk _PTR_ next;
    int_32              used; /* offset from beginning of receive buffer */
    int_32              size; /* chunk size in bytes */

} Rchunk;

/*  Transmission Control Block (TCB)
 *  This is TCP's central data structure; one TCB is allocated to each
 *   TCP connection.
 */
typedef struct tcb_struct {
   struct tcb_struct _PTR_    next;             /* TCP server maintained list of TCB's */

   uint_32  SOCKET;           /* Upper layer socket structure */
   uint_32  VALID;            /* this TCB is valid if = TCB_VALID_ID */
                                                /* and validly closed if = TCB_INVALID_ID */
   boolean  bypass_rx_chksum; /* TRUE to bypass checksum check on Rx */
   boolean  bypass_tx_chksum; /* TRUE to bypass checksum calc. on Tx */

#if 0
   TCP_PARM_PTR         oclient;
                             /* current OPEN/CLOSE?/WAIT request
                                or NULL if no control request
                                is pending; only one such non-I/O request
                                can be done at a time. */

   uint_16   RESERVED1655;
   uint_16   oevents;        /* event(s) oclient is waiting for... */
#endif

   uint_16   state;          /* state of connection */

   uint_16   status;         /* bit-wise status of connection (TCPS_*) */
   uint_16   options;        /* TcpOpen() options (TCPO_*) */

   uchar     RESERVED3;
   uchar     tos;            /* IP type-of-service (includes precedence) */

   int_32    lasterror;      /* last error code on IP_Send(), or last soft
                                error; cleared when read by oclient */

   uint_32   send_error;     /* last error on sending data  */

   _ip_address  local_host;    /* local host; may be INADDR_ANY in LISTEN state */
   _ip_address  remote_host;   /* remote host; may be INADDR_ANY in LISTEN state */

#if RTCSCFG_ENABLE_IP6
   in6_addr  local_ipv6_host;  /* local IPv6  host; may be INADDR_ANY in LISTEN state */
   in6_addr  remote_ipv6_host; /* remote IPv6 host; may be INADDR_ANY in LISTEN state */
   uint_32   if_scope_id;      /* interface number, scope_id */
#endif
   uint_16   af;               /* inet family */ 


   uint_16   local_port;     /* local TCP port [...] */
   uint_16   remote_port;    /* remote TCP port, may be 0 in LISTEN state */

#if 0
   /*  the following four are used to return to LISTEN state...  */
   _ip_address  i_local_host;   /* initial local_host */
   _ip_address  i_remote_host;  /* initial remote_host */
   uint_16   i_local_port;   /* initial local_port */
   uint_16   i_remote_port;  /* initial remote_port */
#endif

   /* receive section */

   Rchunk _PTR_ rcvchunks;   /* portions of receive buffer which we have
                                (since we keep out-of-order packets' data)*/
   uchar _PTR_ rcvbuf;       /* current receive buffer (equals rcvringbuf or
                                rcvuserbuf); it is a circular buffer,
                                except when it equals rcvuserbuf ... */
   uint_32     rcvlen;       /* size of receive buffer */
   uint_32     rcvpos;       /* position within rcvbuf (0..rcvlen-1)
                                of next byte to be received by client;
                                always zero if rcvbuf==rcvuserbuf */
   uchar _PTR_ rcvringbuf;   /* default circular receive buffer */
   uint_32     rcvringlen;   /* total size of rcvringbuf */
   uchar _PTR_ rcvuserbuf;   /* (user) buffer of Receive client, if any */
   uint_32     rcvuserlen;   /* total size of rcvuserbuf */


   uint_32   rcvusernxt;     /* nb of data bytes in rcvuserbuf; undefined in
                                the special case when rcvuserbuf==rcvbuf;
                                otherwise, if rcvuserbuf!=0 then the
                                implementation insures that whatever
                                contiguous data is available at the head
                                of the receive buffer has been put in the
                                user buffer, which implies that either the
                                data in rcvbuf is non-contiguous with that
                                in rcvuserbuf, or that rcvuserbuf is full */

   TCP_PARM_PTR   rcvHead;   /* First receive currently active */
   TCP_PARM_PTR   rcvTail;   /* Last receive currently active */

   uint_16   conn_pending;   /* 1 = conn established but not accepted */
   uint_16   rcvevents;      /* options as specified in TcpReceive();
                                may contain TCPS_RPUSH, ? */

   int_32    rcvto;          /* user Receive() timeout (msecs) or 0 */
   uint_32   rcvbufseq;      /* sequence # of 1st byte in rcvbuf */
                             /* The quantity  rcvbufseq + rcvlen  should
                                always increase monotonically (to avoid the
                                `shrinking window' misbehaviour) */
   uint_32   rcvnxt;         /* next (contiguous) sequence # to receive */
   uint_32   rcvwndedge;     /* right edge of advertised receive window
                                (bytes available in receive buffer for peer
                                TCP); left edge is rcvnxt, so size of
                                advertised rcv. wnd. is rcvwndedge - rcvnxt;
                                this size may be less than the real receive
                                window (because of SWS-avoidance and the
                                0xffff and rcvwndmax limits), which is
                                always rcvbufseq + rcvlen - rcvnxt */
   uint_32   rcvwndmax;      /* maximum window to advertise; usually 0xffff,
                                but may be made 0x7fff for peer TCP's which
                                don't properly handle large advertised
                                windows, or be made the size of a packet
                                to avoid being sent back to back packets
                                */


   uint_32   rcvup;          /* 1st seq # following received urgent data;
                                valid if (options & TCPS_RURGENT) != 0) */
   uint_32   rcvpush;        /* 1st seq # following last received PSH;
                                valid if (options & TCPS_RPUSH) != 0) */
   uint_32   rcvirs;         /* initial receive sequence # */

   uint_32   rcvuna;         /* (1st) sequence # unacknowledged; used for
                                delayed ACKs (TCP_ACKDELAY_DEFAULT ms or
                                until rcv2full bytes) */


   TcbTimeQ  rcvtq;          /* user timer, for receive timeout */


   uint_16   RESERVED5;
   uint_16   rcvmss;         /* maximum segment size for receiving (mss_r);
                                any IP or TCP options must be subtracted
                                from this value; computed at connection
                                setup and doesn't change */

   uint_32   rcvmax;         /* biggest segment received (data only), but at
                                most rcvmss; used to compute rcv2full */
   uint_32   rcv2full;       /* 1.75 times rcvmax, this is the amount of
                                data received which always triggers sending
                                an ACK; initially 1, it is computed from
                                rcvmax when the latter changes */

   /* send section */

   SbufNode _PTR_ sndbuf;    /* head of send buffer chain (0 if empty) */


   SbufNode _PTR_ sndbuftail;/* tail of send buffer chain (0 if empty) */
   uint_32   sndlen;         /* total size of send buffers */

   uchar _PTR_ sndringbuf;   /* default circular transmit buffer */
   uint_32   sndringlen;     /* length of sndringbuf */
   uint_32   sndringhead;    /* next avail. byte within sndringbuf */
   uint_32   sndringtail;    /* follows last avail. byte within sndringbuf */
   uint_32   sndringavail;   /* nb of bytes available in sndringbuf */
   uint_32   sndbufseq;      /* sequence # of 1st byte of sndbuf */
                             /* The quantity  sndbufseq + sndlen  should
                                always increase monotonically */

   uint_32   snduna;         /* (1st) sequence # unacknowledged */
   uint_32   sndxmit;        /* sequence # next to (re)transmit */
   int_32    sndrto;         /* retransmission timeout (RTO) in msecs */
                             /* ^ expon. backed off by 2's... */
   int_32    sndrtomax;      /* maximum value (clamp) for sndrto */
   int_32    sndprobeto;     /* retransmission timeout for 0-window probes */
   int_32    sndprobetomax;  /* maximum value (clamp) for sndproberto */
   int_32    sndrtta;        /* RTT average estimate; scaled by 2^3 */
   int_32    sndrttd;        /* RTT deviation estimate; scaled by 2^2 */

   TcbTimeQ  sndxmittq;      /* retransmit timer, for next packet to rexmit;
                                also used for TIME_WAIT delay, and for
                                probe retransmits (see sndxmitwhat) */

   uint_16   keepalive;      /* when nonzero, next call to TCP_Transmit()
                                must send a keepalive probe */
   char      RESERVED7;
   char      sndxmitwhat;    /* xmit: 0==idle/timewait, 1==probe, 2==xmit */
   boolean   sndtohard;      /* true if sndtq active on sndto_2, that is,
                                soft timeout expired, hard timeout coming*/
   int_32    timewaitto;     /* TIME_WAIT timeout (msecs) */
   int_32    keepaliveto;    /* keepalive period (msecs), or 0 if none */
   
   uint_32   delayackto;     /* DELAY_ACK timeout (msec) */

   TcbTimeQ  sndacktq;       /* transmit timer for delayed ACKs; actually,
                                this is generally used (TCP_Timer_oneshot_max())
                                when "something must be sent"
                                within perhaps a certain amount of time */

   int_32    sndto_1;        /* user (soft) timeout value to use for sndtq,
                                0 if none */
   int_32    sndto_2;        /* hard timeout value for sndtq, or 0 if none;
                                this timeout is started after sndtq expires
                                on sndto_1, so it is a delta from sndto_1;
                                if sndto_1 is zero, sndto_2 is also zero */
   TcbTimeQ  sndtq;          /* timer for timeout on ack of transmissions */

   SndClock _PTR_ sndclk;    /* packet transmission times, used for
                                calculating RTT (round trip time) */
   SndClock _PTR_ sndclkend; /* sndclk + sndclks */
   SndClock _PTR_ sndclkhead;/* circular queue of entries in sndclk[]... */
   SndClock _PTR_ sndclktail;/*    "    */
   int_16    sndclks;        /* size of sndclk[] (nb of entries) */
   int_16    sndclkavail;    /* nb of available entries in sndclk[] queue */

   uint_32   sndnxt;         /* 1st sequence # of unsent data (next new data
                                to send) */
   uint_32   sndwnd;         /* window allowed to send (from snduna) */

   int_32    sndcwnd;        /* congestion window, in bytes, for Jacobson's
                                combined `slow-start' and congestion
                                avoidance (see [1]) */
   int_32    sndstresh;      /* window size threshold for switching between
                                `slow-start' and congestion avoidance
                                (again, see [1]) */
   uint_32   sndwl1;         /* sequence # of last seg. to update sndwnd */
   uint_32   sndwl2;         /* ack # of last seg. to update sndwnd */

   uint_16   sndmss;         /* maximum segment size for sending (mss_s);
                                the length of any IP or TCP options sent
                                (but not headers) must be subtracted from
                                this value */
   uint_16   sndmax;         /* maximum data bytes to send in a segment;
                                this is sndmss - (length of IP and TCP
                                options sent with data) */
   uint_32   sndup;          /* urgent pointer sequence # (1st byte after);
                                valid if (options & TCPS_TURGENT) != 0) */
   uint_32   sndpush;        /* seq # of 1st byte after last PSH to send;
                                valid if (options & TCPS_TPUSH) != 0) */
   uint_32   sndiss;         /* initial send sequence # */

   /* statistics */
   uint_32   sndwndmax;      /* max value of sndwnd during connection */
   uint_32   sndcwndmax;     /* max value of sndcwnd during connection */
   uint_32   rexmts;         /* number of retransmissions */
   uint_32   sndtmouts;      /* number of send timeouts */

   struct tcb_struct _PTR_     LISTEN_TCB;  /* The listening TCB which was cloned */
   struct tcb_struct _PTR_     NOSOCK_NEXT; /* A linked list of TCBs spawned from
                                        listening TCB */
   uint_32   tcb_spawn_time; /* keep the time when tcb is created */

   RTCS_LINKOPT_STRUCT        TX;
   boolean   tcpsecuredraft0; /* invoke draft-ietf-tcpm-tcpsecure-00.txt processing */
   int_32    ackmodifier;     /* used by tcpsecure (above) processing to sometimes decriment value being ack'ed */
} TCB_STRUCT, _PTR_ TCB_STRUCT_PTR;

/*
** TCP Configuration
**    This information is persistent for the TCP layer.
*/
typedef struct tcp_cfg_struct {
#if RTCSCFG_ENABLE_TCP_STATS
   TCP_STATS      STATS;         /* Statistics */
#endif

   TCB_STRUCT_PTR TCBhead;       /* List of active TCBs */
   Rchunk _PTR_   Rchunkhead;    /* List of active Rchunks */
   Rchunk _PTR_   Rchunkfree;    /* List of free Rchunks */
   SbufNode _PTR_ SbufNodehead;  /* List of active SbufNodes */
   SbufNode _PTR_ SbufNodefree;  /* List of free SbufNodes */

   uint_32        nextno;        /* for TCB->no */
   ICB_STRUCT_PTR icb;           /* IP "connection" for TCP protocol */

   uint_16        RESERVED1;
   uint_16        next_port;     /* next port to assign
                                    for TcpOpen with localport==0 */

   TimeQ _PTR_    qh;            /* the timeout queue head */
   uint_32        lasttime;      /* time TCP_Tick_server() was last called */

   TimeQ _PTR_ _PTR_ qhead;      /* Address of the timeout queue head */

   TcbTimeQ _PTR_ acktq;         /* expired delayed-ACK timers */
   TcbTimeQ _PTR_ xmittq;        /* expired rexmit/probe/timewait timers*/
   TcbTimeQ _PTR_ sndtq;         /* expired ack-of-transmission timers */
   TcbTimeQ _PTR_ rcvtq;         /* expired receive timeout timers */

   TCP_PARM_PTR   OPENS;         /* pending open requests */
   TCPIP_EVENT    EVENT_TO_DEQUEUE_PCB; /*
                                           This event is used to
                                           dequeue an enqueued PCB
                                           (for simultaneos SYN problem)
                                           if it remains on the queue for
                                           more than 1 second
                                        */

   RTCSPCB_PTR       DEFER_HEAD;
   RTCSPCB_PTR _PTR_ DEFER_TAIL;

   uint_32        CONN_COUNT;    /* current connection count */

   uint_32        DEFER_COUNT;   /* number of deferred PCBs */

   TCB_STRUCT_PTR NOSOCK_TCB;    /* List of TCBs spawned from listening TCB */

   uint_32         HALF_OPEN_TCB_COUNT;   /* current half open TCB count */
#if RTCSCFG_TCP_MAX_HALF_OPEN
   TCB_STRUCT_PTR  HALF_OPEN_TCB_LIST[RTCSCFG_TCP_MAX_HALF_OPEN];
                                 /* array of half open TCBs */
#endif

} TCP_CFG_STRUCT, _PTR_ TCP_CFG_STRUCT_PTR;

/************************************************************************/
/* Functions                                                            */
/************************************************************************/

extern uint_32 TCP_Init (void);

extern void TCP_Process_create  (TCP_PARM_PTR);
extern void TCP_Process_bind    (TCP_PARM_PTR);

extern void TCP_Process_open    (TCP_PARM_PTR);

extern void TCP_Process_accept  (TCP_PARM_PTR);

extern void TCP_Process_send    (TCP_PARM_PTR);
extern void TCP_Process_receive (TCP_PARM_PTR);

extern void TCP_Process_close   (TCP_PARM_PTR);
extern void TCP_Process_abort   (TCP_PARM_PTR);

extern void TCP_Process_signal  (void);

extern void TCP_Service_packet
(
   RTCSPCB_PTR          ,        /* IN/OUT - the receive packet */
   pointer                       /* IN/OUT - TCP layer constants */
);
extern void TCP6_Service_packet
   (
      RTCSPCB_PTR    pcb,     /* IN/OUT - the receive packet */
      void _PTR_     cfg      /* IN/OUT - TCP layer constants */
   );
extern void TCP4_Service_packet
   (
      RTCSPCB_PTR    pcb,     /* IN/OUT - the receive packet */
      void _PTR_     cfg      /* IN/OUT - TCP layer constants */
   ); 
   
#endif
/* EOF */
