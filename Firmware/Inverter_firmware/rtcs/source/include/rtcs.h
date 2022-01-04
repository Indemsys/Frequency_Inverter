#ifndef __rtcs_h__
#define __rtcs_h__
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
* $FileName: rtcs.h$
* $Version : 3.8.42.0$
* $Date    : Oct-1-2012$
*
* Comments:
*
*   This file contains the defines, externs and data
*   structure definitions required by application
*   programs in order to use the RTCS Communication Library.
*
*END************************************************************************/

#include <rtcsvers.h>
#include <rtcsrtos.h>
#include <rtcslog.h>
#include <rtcs_in.h>
#include <string.h>

/*
** Constants
*/

/*
** Protocols for the RTCS_protocol_table[]
*/
#define RTCSPROT_IGMP   IGMP_init
#define RTCSPROT_UDP    UDP_init
#define RTCSPROT_TCP    TCP_Init
#define RTCSPROT_RIP    RIP_init
#define RTCSPROT_OSPF   OSPF_init
#define RTCSPROT_IPIP   IPIP_init

/*
** Protocol and Address families
*/
#define PF_INET         1
#define AF_UNSPEC       0
#define AF_INET         1

#define AF_INET6         28         /* IPv6 */
#define PF_INET6         AF_INET6   /* Same as the address family. */

/*
 * Flag values for getaddrinfo()
 */
#define	AI_PASSIVE	    0x00000001 /* get address to use bind() */
#define	AI_CANONNAME	0x00000002 /* fill ai_canonname */
#define	AI_NUMERICHOST	0x00000004 /* prevent host name resolution */

/*
** Socket types
*/
#define SOCK_DGRAM      ((uint_32)&SOCK_DGRAM_CALL)
#define SOCK_STREAM     ((uint_32)&SOCK_STREAM_CALL)

/*
** Validity check for socket structures
*/
#define SOCKET_VALID       0x52544353  /* "rtcs" */

/*
** Default number of tasks that can simultaneously own a socket
*/
#define SOCKET_NUMOWNERS   8

/*
** protocol level at which option resides
*/
#define SOL_SOCKET      ((uint_32)&SOL_SOCKET_CALL)
#define SOL_IP          ((uint_32)&SOL_IP_CALL)
#define SOL_UDP         ((uint_32)&SOL_UDP_CALL)
#define SOL_TCP         ((uint_32)&SOL_TCP_CALL)
#define SOL_IGMP        ((uint_32)&SOL_IGMP_CALL)
#define SOL_LINK        ((uint_32)&SOL_LINK_CALL)
#define SOL_NAT         ((uint_32)&SOL_NAT_CALL)



/*
**   socket option ids
*/
#define OPT_CONNECT_TIMEOUT            (0x01)
#define OPT_RETRANSMISSION_TIMEOUT     (0x02)
#define OPT_SEND_TIMEOUT               (0x03)
#define OPT_RECEIVE_TIMEOUT            (0x04)
#define OPT_RECEIVE_PUSH               (0x05)
#define OPT_SEND_NOWAIT                (0x06)
#define OPT_SEND_WAIT_OBSOLETE         (0x07) /* obsolete */
#define OPT_SEND_PUSH                  (0x08)
#define OPT_RECEIVE_NOWAIT             (0x09)
#define OPT_CHECKSUM_BYPASS            (0x0A)
#define OPT_ACTIVE                     (0x0B)
#define OPT_TBSIZE                     (0x0C)
#define OPT_RBSIZE                     (0x0D)
#define OPT_MAXRTO                     (0x0E)
#define OPT_MAXRCV_WND                 (0x0F)
#define OPT_KEEPALIVE                  (0x10)
#define OPT_NOWAIT                     (0x11)
#define OPT_NO_NAGLE_ALGORITHM         (0x12)
#define OPT_NOSWRBUF                   (0x13)
#define OPT_CALL_BACK_OBSOLETE         (0x14) /* obsolete */
#define OPT_SOCKET_TYPE                (0x15)
#define OPT_CALLBACK                   (0x16)
#define OPT_TIMEWAIT_TIMEOUT           (0x17)
#define OPT_TCPSECUREDRAFT_0           (0x18)
#define OPT_DELAY_ACK                  (0x19)

#define OPT_SOCKET_ERROR               (0x1c)

/*
**   default option values
*/
#define DEFAULT_CONNECT_TIMEOUT        (480000L)   /* 8 mins */
#define DEFAULT_RETRANSMISSION_TIMEOUT (3000)      /* 3 sec  */
#define DEFAULT_SEND_TIMEOUT           (DEFAULT_CONNECT_TIMEOUT/2)
#define DEFAULT_RECEIVE_TIMEOUT        (0)         /* no timeout */
#define DEFAULT_NOWAIT                 FALSE
#define DEFAULT_SEND_NOWAIT            FALSE
#define DEFAULT_SEND_WAIT              FALSE
#define DEFAULT_RECEIVE_NOWAIT         FALSE
#define DEFAULT_WAIT                   FALSE       /* don't wait for ack */
#define DEFAULT_PUSH                   TRUE        /* push */
#define DEFAULT_CHECKSUM_BYPASS        FALSE       /* perform checksum */
#define DEFAULT_ACTIVE                 TRUE
#define DEFAULT_TBSIZE                 (-1)
#define DEFAULT_RBSIZE                 (-1)
#define DEFAULT_MAXRTO                 (0)
#define DEFAULT_MAXRCV_WND             (0)
#define DEFAULT_KEEPALIVE              (0)
#define DEFAULT_NO_NAGLE_ALGORITHM     FALSE
#define DEFAULT_NOSWRBUF               FALSE
#define DEFAULT_CALL_BACK              NULL
#define DEFAULT_CALLBACK               NULL
#define DEFAULT_TIMEWAIT_TIMEOUT       (2*1000) /* msec */ /* 0 = default 4 mins.*/
#define DEFAULT_TCPSECUREDRAFT_0       FALSE
#define DEFAULT_DELAY_ACK               (TCP_ACKDELAY)


/*
** Socket options
*/
#define RTCS_SO_TYPE                   OPT_SOCKET_TYPE
#define RTCS_SO_ERROR                  OPT_SOCKET_ERROR

#define RTCS_SO_IGMP_ADD_MEMBERSHIP    0
#define RTCS_SO_IGMP_DROP_MEMBERSHIP   1
#define RTCS_SO_IGMP_GET_MEMBERSHIP    2

#define RTCS_SO_IP_RX_DEST             0
#define RTCS_SO_IP_RX_TTL              1
#define RTCS_SO_IP_RX_TOS              2
#define RTCS_SO_IP_TX_TTL              3
#define RTCS_SO_IP_LOCAL_ADDR          4
#define RTCS_SO_IP_TX_TOS              5

#define RTCS_SO_LINK_TX_8023           0
#define RTCS_SO_LINK_TX_8021Q_PRIO     1
#define RTCS_SO_LINK_RX_8023           2
#define RTCS_SO_LINK_RX_8021Q_PRIO     3

#define RTCS_SO_UDP_NONBLOCK           OPT_SEND_NOWAIT
#define RTCS_SO_UDP_NONBLOCK_TX        OPT_SEND_NOWAIT
#define RTCS_SO_UDP_NONBLOCK_RX        OPT_RECEIVE_NOWAIT
#define RTCS_SO_UDP_NOCHKSUM           OPT_CHECKSUM_BYPASS

#define RTCS_SO_NAT_TIMEOUTS           0
#define RTCS_SO_NAT_PORTS              1

/*
** Flags for send[to]/recv[from]
*/
#define RTCS_MSG_O_NONBLOCK   0x0001
#define RTCS_MSG_S_NONBLOCK   0x0002
#define RTCS_MSG_BLOCK        (RTCS_MSG_O_NONBLOCK)
#define RTCS_MSG_NONBLOCK     (RTCS_MSG_O_NONBLOCK | RTCS_MSG_S_NONBLOCK)
#define RTCS_MSG_O_NOCHKSUM   0x0004
#define RTCS_MSG_S_NOCHKSUM   0x0008
#define RTCS_MSG_CHKSUM       (RTCS_MSG_O_NOCHKSUM)
#define RTCS_MSG_NOCHKSUM     (RTCS_MSG_O_NOCHKSUM | RTCS_MSG_S_NOCHKSUM)
#define RTCS_MSG_PEEK         0x0010
#define RTCS_MSG_NOLOOP       0x0020

/*
** Flags for logging
*/
#define RTCS_LOGCTRL_FUNCTION    0x00000001
#define RTCS_LOGCTRL_PCB         0x00000002
#define RTCS_LOGCTRL_TIMER       0x00000004
#define RTCS_LOGCTRL_ALL         0x00000007

#define RTCS_LOGCTRL_IFTYPE(p)   ((p) & 0x3FF)
#define RTCS_LOGCTRL_ARP(p)      ((p) & 0x3FF | 0x4000)
#define RTCS_LOGCTRL_PROTO(p)    ((p) & 0x3FF | 0x8000)
#define RTCS_LOGCTRL_PORT(p)     ((p) & 0x3FF | 0xC000)

/*
** Close methods for shutdown()
*/
#define FLAG_ABORT_CONNECTION       (0x0010)
#define FLAG_CLOSE_TX               (0x0001)


/*
** Options for RTCS_load()
*/
#define RTCSLOADOPT_EXEC      0x01


/*
** Modes for the FTP client
*/
#define FTPMODE_DEFAULT 1
#define FTPMODE_PORT    2
#define FTPMODE_PASV    3
#define FTPDIR_RECV     0
#define FTPDIR_SEND     4


/*
** Successful return code.  No errors.
*/
#define RTCS_OK                     0
#define RTCS_ERROR                  (-1)
#define RTCS_HANDLE_ERROR           (0xffffffffL)
#define RTCS_SOCKET_ERROR           (0xffffffffL)


/*
** RTCS Error Code Definitions
*/

/*
** Generic Error codes
*/
#define RTCSERR_OUT_OF_MEMORY           (RTCS_ERROR_BASE|0x102)
#define RTCSERR_TIMEOUT                 (RTCS_ERROR_BASE|0x103)
#define RTCSERR_INVALID_ADDRESS         (RTCS_ERROR_BASE|0x104)
#define RTCSERR_INVALID_PARAMETER       (RTCS_ERROR_BASE|0x105)
#define RTCSERR_READ_ABORTED            (RTCS_ERROR_BASE|0x11f)
#define RTCSERR_OUT_OF_BUFFERS          (RTCS_ERROR_BASE|0x120)
#define RTCSERR_CREATE_PARTITION_FAILED (RTCS_ERROR_BASE|0x121)
#define RTCSERR_OUT_OF_SOCKETS          (RTCS_ERROR_BASE|0x122)
#define RTCSERR_FOPEN_FAILED            (RTCS_ERROR_BASE|0x123)
#define RTCSERR_FEATURE_NOT_ENABLED     (RTCS_ERROR_BASE|0x124)
#define RTCSERR_SEND_FAILED             (RTCS_ERROR_BASE|0x132)
#define RTCSERR_CREATE_POOL_FAILED      (RTCS_ERROR_BASE|0x135)
#define RTCSERR_OPEN_QUEUE_FAILED       (RTCS_ERROR_BASE|0x136)
#define RTCSERR_CREATE_FAILED           (RTCS_ERROR_BASE|0x137)
#define RTCSERR_RECEIVE_FAILED          (RTCS_ERROR_BASE|0x138)
#define RTCSERR_DEALLOC_FAILED          (RTCS_ERROR_BASE|0x139)
#define RTCSERR_SERVER_ALREADY_RUNNING  (RTCS_ERROR_BASE|0x140)
#define RTCSERR_SERVER_NOT_RUNNING      (RTCS_ERROR_BASE|0x141)

/*
** PPP error codes
*/
#define PPP_OK                                RTCS_OK
#define RTCSERR_PPP_ALLOC_FAILED              (RTCS_ERROR_BASE|0x200)
#define RTCSERR_PPP_INIT_MUTEX_FAILED         (RTCS_ERROR_BASE|0x201)
#define RTCSERR_PPP_CREATE_CTRL_POOL_FAILED   (RTCS_ERROR_BASE|0x202)
#define RTCSERR_PPP_CREATE_PKT_POOL_FAILED    (RTCS_ERROR_BASE|0x203)
#define RTCSERR_PPP_CREATE_RX_FAILED          (RTCS_ERROR_BASE|0x204)
#define RTCSERR_PPP_CREATE_TX_FAILED          (RTCS_ERROR_BASE|0x205)
#define RTCSERR_PPP_INVALID_CALLBACK          (RTCS_ERROR_BASE|0x206)
#define RTCSERR_PPP_INVALID_HANDLE            (RTCS_ERROR_BASE|0x240)
#define RTCSERR_PPP_LINK_NOT_OPEN             (RTCS_ERROR_BASE|0x241)
#define RTCSERR_PPP_PACKET_TOO_SHORT          (RTCS_ERROR_BASE|0x242)
#define RTCSERR_PPP_PACKET_TOO_LONG           (RTCS_ERROR_BASE|0x243)
#define RTCSERR_PPP_OUT_OF_BUFFERS            (RTCS_ERROR_BASE|0x244)
#define RTCSERR_PPP_INVALID_PROTOCOL          (RTCS_ERROR_BASE|0x245)
#define RTCSERR_PPP_ALLOC_CALL_FAILED         (RTCS_ERROR_BASE|0x246)
#define RTCSERR_PPP_PROT_NOT_FOUND            (RTCS_ERROR_BASE|0x247)
#define RTCSERR_PPP_FSM_ACTIVE                (RTCS_ERROR_BASE|0x280)
#define RTCSERR_PPP_OPEN_QUEUE_FAILED         (RTCS_ERROR_BASE|0x2C0)

/*
** Initialization failures
*/
#define RTCSERR_INITIALIZED      (RTCS_ERROR_BASE|0x300)    /* RTCS already initialized */

/*
** PCB module errors
*/
#define RTCSERR_PCB_ALLOC        (RTCS_ERROR_BASE|0x310)    /* couldn't allocate PCBs         */
#define RTCSERR_PCB_FORK         (RTCS_ERROR_BASE|0x311)    /* PCB already forked             */
#define RTCSERR_PCB_LAYER        (RTCS_ERROR_BASE|0x312)    /* too few PCB layers             */
#define RTCSERR_PCB_BUFFER       (RTCS_ERROR_BASE|0x313)    /* PCB buffer too small           */
#define RTCSERR_PCB_FRAG         (RTCS_ERROR_BASE|0x314)    /* too few PCB fragments          */
#define RTCSERR_PCB_NOFRAG       (RTCS_ERROR_BASE|0x315)    /* can't add fragment (must fork) */
#define RTCSERR_PCB_DEPEND       (RTCS_ERROR_BASE|0x316)    /* can't create dependency        */

/*
** ARP errors
*/
#define RTCSERR_ARP_CFG          (RTCS_ERROR_BASE|0x400)    /* couldn't allocate state       */
#define RTCSERR_ARP_CACHE        (RTCS_ERROR_BASE|0x401)    /* couldn't allocate cache entry */
#define RTCSERR_ARP_CANT_RESOLVE (RTCS_ERROR_BASE|0x402)    /* couldn't resolve address      */
#define RTCSERR_ARP_BAD_HEADER   (RTCS_ERROR_BASE|0x403)    /* error in header               */

/*
** IPCP errors
*/
#define RTCSERR_IPCP_CFG         (RTCS_ERROR_BASE|0x410)    /* couldn't allocate state */

/*
** IP errors
*/
#define RTCSERR_IP_ICB_ALLOC     (RTCS_ERROR_BASE|0x500)    /* couldn't allocate ICB       */
#define RTCSERR_IP_PROT_OPEN     (RTCS_ERROR_BASE|0x501)    /* protocol already open       */
#define RTCSERR_IP_IF_ALLOC      (RTCS_ERROR_BASE|0x502)    /* couldn't allocate interface */
#define RTCSERR_IP_ROUTE_ALLOC   (RTCS_ERROR_BASE|0x503)    /* couldn't allocate route     */
#define RTCSERR_IP_GATE_ALLOC    (RTCS_ERROR_BASE|0x504)    /* couldn't allocate gateway   */
#define RTCSERR_IP_BIND_ADDR     (RTCS_ERROR_BASE|0x505)    /* invalid local address       */
#define RTCSERR_IP_BIND_MASK     (RTCS_ERROR_BASE|0x506)    /* invalid network mask        */
#define RTCSERR_IP_SEC_ALLOC     (RTCS_ERROR_BASE|0x507)    /* couldn't allocate SPD node   */
#define RTCSERR_IP_VIRTUAL_ALLOC (RTCS_ERROR_BASE|0x508)    /* couldn't alloc virtual route */

/*
** IP datagram processing failures
*/
#define RTCSERR_IP_UNREACH       (RTCS_ERROR_BASE|0x510)    /* no route to host            */
#define RTCSERR_IP_TTL           (RTCS_ERROR_BASE|0x511)    /* TTL expired                 */
#define RTCSERR_IP_SMALLMTU      (RTCS_ERROR_BASE|0x512)    /* packet exceeds MTU          */
#define RTCSERR_IP_CANTFRAG      (RTCS_ERROR_BASE|0x513)    /* need to fragment but DF set */
#define RTCSERR_IP_BAD_HEADER    (RTCS_ERROR_BASE|0x514)    /* error in header             */
#define RTCSERR_IP_BAD_ADDRESS   (RTCS_ERROR_BASE|0x515)    /* illegal source or dest      */
#define RTCSERR_IP_BAD_CHECKSUM  (RTCS_ERROR_BASE|0x516)    /* invalid checksum            */

/*
** IPIP errors
*/
#define RTCSERR_IPIP_NOT_INITIALIZED   (RTCS_ERROR_BASE|0x550)   /* IPIP not initialized      */
#define RTCSERR_IPIP_LOOP              (RTCS_ERROR_BASE|0x551)   /* received a packet we sent */

/*
** ICMP errors
*/
#define RTCSERR_ICMP_ECHO_TIMEOUT   (RTCS_ERROR_BASE|0x580) /* timed out waiting for echo reply */
#define RTCSERR_ICMP_BAD_HEADER     (RTCS_ERROR_BASE|0x581) /* error in header                  */
#define RTCSERR_ICMP_BAD_CHECKSUM   (RTCS_ERROR_BASE|0x582) /* invalid checksum                 */

/*
** IGMP errors
*/
#define RTCSERR_IGMP_CFG            (RTCS_ERROR_BASE|0x5C0) /* couldn't allocate state        */
#define RTCSERR_IGMP_GROUP_ALLOC    (RTCS_ERROR_BASE|0x5C1) /* couldn't allocate MCB          */
#define RTCSERR_IGMP_GROUP_FREE     (RTCS_ERROR_BASE|0x5C2) /* couldn't free MCB              */
#define RTCSERR_IGMP_INVALID_IP     (RTCS_ERROR_BASE|0x5C3) /* can't join group (nonlocal IP) */
#define RTCSERR_IGMP_NOT_JOINED     (RTCS_ERROR_BASE|0x5C4) /* can't leave group (not joined) */
#define RTCSERR_IGMP_BAD_HEADER     (RTCS_ERROR_BASE|0x5C5) /* error in header                */
#define RTCSERR_IGMP_BAD_CHECKSUM   (RTCS_ERROR_BASE|0x5C6 )/* invalid checksum               */

/*
** UDP errors
*/
#define RTCSERR_UDP_UCB_ALLOC    (RTCS_ERROR_BASE|0x660)    /* couldn't allocate UCB */
#define RTCSERR_UDP_UCB_FREE     (RTCS_ERROR_BASE|0x661)    /* couldn't free UCB     */
#define RTCSERR_UDP_UCB_CLOSE    (RTCS_ERROR_BASE|0x662)    /* UCB not open          */
#define RTCSERR_UDP_PORT_OPEN    (RTCS_ERROR_BASE|0x663)    /* port already open     */
#define RTCSERR_UDP_PORT_ALLOC   (RTCS_ERROR_BASE|0x664)    /* no more ports         */
#define RTCSERR_UDP_PORT_ZERO    (RTCS_ERROR_BASE|0x665)    /* can't send to port 0  */
#define RTCSERR_UDP_BAD_HEADER   (RTCS_ERROR_BASE|0x666)    /* error in header       */
#define RTCSERR_UDP_BAD_CHECKSUM (RTCS_ERROR_BASE|0x667)    /* invalid checksum      */
#define RTCSERR_UDP_SOCKET_CLOSE (RTCS_ERROR_BASE|0x668)    /* socket closed by tasks other than waiting task */


/*
** TCPIP error codes
*/
#define RTCSERR_TCPIP_INVALID_ARGUMENT  (RTCS_ERROR_BASE|0x610)  /* Invalid argument            */
#define RTCSERR_TCPIP_DESTADDR_REQUIRED (RTCS_ERROR_BASE|0x611)  /* Destination address         */
                                                                 /*    required                 */
#define RTCSERR_TCPIP_NO_BUFFS          (RTCS_ERROR_BASE|0x612)  /* No buffer space available   */
#define RTCSERR_TCPIP_DELAY_REQUESTED   (RTCS_ERROR_BASE|0x613)  /* TCP_Delay() called, but not */
                                                                 /*    supported                */
#define RTCSERR_TCPIP_TIMER_CORRUPT     (RTCS_ERROR_BASE|0x614)  /* Corrupt timer pointers      */

/*
** TCP error codes
*/
#define RTCSERR_TCP_OPEN_FAILED         (RTCS_ERROR_BASE|0x630)  /* TcpOpen failed              */
#define RTCSERR_TCP_INVALID_OPTION      (RTCS_ERROR_BASE|0x631)  /* Option was invalid          */
#define RTCSERR_TCP_IN_PROGRESS         (RTCS_ERROR_BASE|0x632)  /* Operation already in        */
                                                                 /*    progress                 */
#define RTCSERR_TCP_ADDR_IN_USE         (RTCS_ERROR_BASE|0x633)  /* Address already in use      */
#define RTCSERR_TCP_ADDR_NA             (RTCS_ERROR_BASE|0x634)  /* Can't assign requested      */
                                                                 /*    address                  */
#define RTCSERR_TCP_CONN_ABORTED        (RTCS_ERROR_BASE|0x635)  /* Software caused connection  */
                                                                 /*    abort                    */
#define RTCSERR_TCP_CONN_RESET          (RTCS_ERROR_BASE|0x636)  /* Connection reset by peer    */
#define RTCSERR_TCP_HOST_DOWN           (RTCS_ERROR_BASE|0x637)  /* Host is down                */
#define RTCSERR_TCP_CONN_CLOSING        (RTCS_ERROR_BASE|0x638)  /* Connection closing          */
#define RTCSERR_TCP_CONN_RLSD           (RTCS_ERROR_BASE|0x639)  /* Connection/TCB released     */
#define RTCSERR_TCP_MISSING_OPEN        (RTCS_ERROR_BASE|0x63A)  /* TCB exists in LISTEN state  */
                                                                 /*    with no associated Listn */
                                                                 /*    request from upper layer */
#define RTCSERR_TCP_CTR_ZERO_RSIZE      (RTCS_ERROR_BASE|0x63B)  /* TCP_Copy_to_ring got 0      */
                                                                 /*    ring buf size            */
#define RTCSERR_TCP_SP_BAD_SEND_STATE   (RTCS_ERROR_BASE|0x63C)  /* Attempted to TCP_Send_packet*/
                                                                 /*    in invalid state         */
#define RTCSERR_TCP_SP_OUT_OF_PCBS      (RTCS_ERROR_BASE|0x63D)  /* Could not get a PCB in      */
                                                                 /*    TCP_Send_packet          */
#define RTCSERR_TCP_SRR_OUT_OF_PCBS     (RTCS_ERROR_BASE|0x63E)  /* Could not get a PCB in      */
                                                                 /*    TCP_Send_reply_reset     */
#define RTCSERR_TCP_SR_OUT_OF_PCBS      (RTCS_ERROR_BASE|0x63F)  /* Could not get a PCB in      */
                                                                 /*    TCP_Send_reset           */
#define RTCSERR_TCP_SHRINKER_HOST       (RTCS_ERROR_BASE|0x640)  /* TCP detected a 'shrinker'   */
                                                                 /*    peer host                */
#define RTCSERR_TCP_PA_BUFF_CORRUPT     (RTCS_ERROR_BASE|0x642)  /* TCP send buffer corruption  */
                                                                 /*    detected in TCP_Process_ack   */
#define RTCSERR_TCP_PS_FAILED_GET_SBUF  (RTCS_ERROR_BASE|0x643)  /* Could not get an SbufNode   */
                                                                 /*    in TCP_Process_send      */
#define RTCSERR_TCP_PR_OUT_OF_MEM       (RTCS_ERROR_BASE|0x644)  /* Could not get a Rcvchunk    */
                                                                 /*    buffer in Setup Receive  */
#define RTCSERR_TCP_PP_OUT_OF_MEM       (RTCS_ERROR_BASE|0x645)  /* Could not add a host IpList */
                                                                 /*    node in TCP_Process_packet    */
#define RTCSERR_TCP_TIMER_FAILURE       (RTCS_ERROR_BASE|0x646)  /* Could not start a TCP timer */
#define RTCSERR_TCP_NOT_CONN            (RTCS_ERROR_BASE|0x647)  /* Socket is not connected     */
#define RTCSERR_TCP_SHUTDOWN            (RTCS_ERROR_BASE|0x648)  /* Can't send after socket     */
                                                                 /*    shutdown                 */
#define RTCSERR_TCP_TIMED_OUT           (RTCS_ERROR_BASE|0x649)  /* Connection timed out        */
#define RTCSERR_TCP_CONN_REFUSED        (RTCS_ERROR_BASE|0x64A)  /* Connection refused          */
#define RTCSERR_TCP_NO_MORE_PORTS       (RTCS_ERROR_BASE|0x64B)  /* No more ports available     */
                                                                 /*    for connections          */
#define RTCSERR_TCP_BAD_STATE_FOR_CLOSE (RTCS_ERROR_BASE|0x64C)  /* Attempted to call           */
                                                                 /*    TCP_Process_effective_close or */
                                                                 /*    TCP_Process_close in invalid  */
                                                                 /*    state                    */
#define RTCSERR_TCP_BAD_STATE_FOR_REL   (RTCS_ERROR_BASE|0x64D)  /* Attempted to release a TCB  */
                                                                 /*    in an invalid state      */
#define RTCSERR_TCP_REXMIT_PROBLEM      (RTCS_ERROR_BASE|0x64E)  /* A retransmission timer      */
                                                                 /*    timed out while in IDLE  */
                                                                 /*    mode (i.e. invalid)      */
#define RTCSERR_TCP_URGENT_DATA_PROB    (RTCS_ERROR_BASE|0x64F)  /* Urgent data pointer         */
                                                                 /*    was corrupted in the tcb */
#define RTCSERR_TCP_DEALLOC_FAILED      (RTCS_ERROR_BASE|0x650)  /* A call to release memory failed   */
#define RTCSERR_TCP_HOST_UNREACH        (RTCS_ERROR_BASE|0x651)  /* No route to host            */
#define RTCSERR_TCP_BAD_HEADER          (RTCS_ERROR_BASE|0x652)  /* error in header             */
#define RTCSERR_TCP_BAD_CHECKSUM        (RTCS_ERROR_BASE|0x653)  /* invalid checksum            */

/*
** Socket error codes
*/
#define RTCSERR_SOCK_INVALID        (RTCS_ERROR_BASE|0x704)
#define RTCSERR_SOCK_INVALID_AF     (RTCS_ERROR_BASE|0x702)
#define RTCSERR_SOCK_SHORT_ADDRESS  (RTCS_ERROR_BASE|0x706)
#define RTCSERR_SOCK_NOT_SUPPORTED  (RTCS_ERROR_BASE|0x719)
#define RTCSERR_SOCK_INVALID_OPTION (RTCS_ERROR_BASE|0x71A)
#define RTCSERR_SOCK_SHORT_OPTION   (RTCS_ERROR_BASE|0x71B)
#define RTCSERR_SOCK_NOT_BOUND      (RTCS_ERROR_BASE|0x709)
#define RTCSERR_SOCK_IS_BOUND       (RTCS_ERROR_BASE|0x705)
#define RTCSERR_SOCK_NOT_LISTENING  (RTCS_ERROR_BASE|0x70C)
#define RTCSERR_SOCK_IS_LISTENING   (RTCS_ERROR_BASE|0x70B)
#define RTCSERR_SOCK_NOT_CONNECTED  (RTCS_ERROR_BASE|0x70D)
#define RTCSERR_SOCK_IS_CONNECTED   (RTCS_ERROR_BASE|0x70A)
#define RTCSERR_SOCK_NOT_OWNER      (RTCS_ERROR_BASE|0x70E)
#define RTCSERR_SOCK_CLOSED         (RTCS_ERROR_BASE|0x70F)

/*
** RTCS_load() TFTP errors
*/
#define RTCSERR_TFTP_SOCKET         (RTCS_ERROR_BASE|0x800)
#define RTCSERR_TFTP_BIND           (RTCS_ERROR_BASE|0x801)
#define RTCSERR_TFTP_RRQ_ALLOC      (RTCS_ERROR_BASE|0x802)
#define RTCSERR_TFTP_RRQ_SEND       (RTCS_ERROR_BASE|0x803)
#define RTCSERR_TFTP_TIMEOUT        (RTCS_ERROR_BASE|0x804)
#define RTCSERR_TFTP_ERROR          (RTCS_ERROR_BASE|0x810)

/*
** RTCS_load() decoder errors
*/

#define RTCSERR_SREC_CHAR           (RTCS_ERROR_BASE|0x880)
#define RTCSERR_SREC_RECORD         (RTCS_ERROR_BASE|0x881)
#define RTCSERR_SREC_SHORT          (RTCS_ERROR_BASE|0x882)
#define RTCSERR_SREC_CHECKSUM       (RTCS_ERROR_BASE|0x883)
#define RTCSERR_SREC_START          (RTCS_ERROR_BASE|0x884)
#define RTCSERR_SREC_END            (RTCS_ERROR_BASE|0x885)
#define RTCSERR_SREC_EOF            (RTCS_ERROR_BASE|0x886)
#define RTCSERR_SREC_ERROR          (RTCS_ERROR_BASE|0x887)

#define RTCSERR_COFF_BAD_MAGIC      (RTCS_ERROR_BASE|0x890)
#define RTCSERR_COFF_ALLOC_FAILED   (RTCS_ERROR_BASE|0x891)
#define RTCSERR_COFF_TRUNCATED      (RTCS_ERROR_BASE|0x892)
#define RTCSERR_COFF_ERROR          (RTCS_ERROR_BASE|0x893)

/*
** SNMP TRAP Interface Error Codes
*/
#define RTCSERR_TRAP_INSERT_FAILED      (RTCS_ERROR_BASE|0x901)  /* TRAP Insertion failed     */
#define RTCSERR_TRAP_REMOVE_FAILED      (RTCS_ERROR_BASE|0x902)  /* TRAP Insertion failed     */

/*
** DHCP error codes
*/
#define RTCSERR_DHCP_ALREADY_INIT           (RTCS_ERROR_BASE|0xA01)
#define RTCSERR_DHCP_MESSAGE_OPTION_MISSING (RTCS_ERROR_BASE|0xA02)
#define RTCSERR_DHCP_SERVER_OPTION_MISSING  (RTCS_ERROR_BASE|0xA03)
#define RTCSERR_DHCP_ADDR_IN_USE            (RTCS_ERROR_BASE|0xA04)
#define RTCSERR_DHCP_PACKET_ERROR           (RTCS_ERROR_BASE|0xA81)
#define RTCSERR_DHCPCLNT_ERROR_DECLINED     (RTCS_ERROR_BASE|0xA82)
#define RTCSERR_DHCPCLNT_XID_MISMATCH       (RTCS_ERROR_BASE|0xA83)
#define RTCSERR_DHCPCLNT_PACKET_SIZE_ERROR  (RTCS_ERROR_BASE|0xA84)

/*
** DNS error codes
*/
#define DNS_OK                                      RTCS_OK
#define RTCSERR_DNS_QUERY_OK                        (RTCS_ERROR_BASE|0xB00)
#define RTCSERR_DNS_NO_NAME_SERVER_RESPONSE         (RTCS_ERROR_BASE|0xB01)
#define RTCSERR_DNS_UNABLE_TO_OPEN_SOCKET           (RTCS_ERROR_BASE|0xB02)
#define RTCSERR_DNS_UNABLE_TO_BIND_SOCKET           (RTCS_ERROR_BASE|0xB03)
#define RTCSERR_DNS_UNABLE_TO_UPDATE_ROOT_SERVERS   (RTCS_ERROR_BASE|0xB04)
#define RTCSERR_DNS_SOCKET_RECV_ERROR               (RTCS_ERROR_BASE|0xB05)
#define RTCSERR_DNS_UNABLE_TO_SEND_QUERY            (RTCS_ERROR_BASE|0xB06)
#define RTCSERR_DNS_NO_RESPONSE_FROM_RESOLVER       (RTCS_ERROR_BASE|0xB07)
#define RTCSERR_DNS_PACKET_RECEPTION_ERROR          (RTCS_ERROR_BASE|0xB08)
#define RTCSERR_DNS_INVALID_NAME                    (RTCS_ERROR_BASE|0xB09)
#define RTCSERR_DNS_INVALID_IP_ADDR                 (RTCS_ERROR_BASE|0xB0A)
#define RTCSERR_DNS_ALL_SERVERS_QUERIED             (RTCS_ERROR_BASE|0xB0B)
#define RTCSERR_DNS_INVALID_LOCAL_NAME              (RTCS_ERROR_BASE|0xB0C)
#define RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY       (RTCS_ERROR_BASE|0xB0D)
#define RTCSERR_DNS_UNABLE_TO_FREE_SOCKET           (RTCS_ERROR_BASE|0xB0E)
#define RTCSERR_DNS_VALID_IP_ADDR                   (RTCS_ERROR_BASE|0xB0F)
#define RTCSERR_DNS_INITIALIZATION_FAILURE          (RTCS_ERROR_BASE|0xB10)
#define RTCSERR_DNS_NOT_IMPLEMENTED                 (RTCS_ERROR_BASE|0xB11)
#define RTCSERR_DNS_UNABLE_TO_CREATE_PARTITION      (RTCS_ERROR_BASE|0xB12)
#define RTCSERR_DNS_SHORT_MESSAGE_ERROR             (RTCS_ERROR_BASE|0xB13)

/*
** NAT error codes
*/
#define RTCSERR_NAT_NO_SESSION                   (RTCS_ERROR_BASE|0xC00)
#define RTCSERR_NAT_UNSUPPORTED_PROTOCOL         (RTCS_ERROR_BASE|0xC01)
#define RTCSERR_NAT_INITIALIZED                  (RTCS_ERROR_BASE|0xC02)
#define RTCSERR_NAT_UNEXPECTED                   (RTCS_ERROR_BASE|0xC03)
#define RTCSERR_NAT_NOT_INITIALIZED              (RTCS_ERROR_BASE|0xC04)
#define RTCSERR_NAT_INVALID_RULE                 (RTCS_ERROR_BASE|0xC05)
#define RTCSERR_NAT_INVALID_PRIVATE_NETWORK      (RTCS_ERROR_BASE|0xC07)
#define RTCSERR_NAT_MAX_PRIVATE_NETWORKS         (RTCS_ERROR_BASE|0xC08)
#define RTCSERR_NAT_DUPLICATE_PRIORITY           (RTCS_ERROR_BASE|0xC09)
#define RTCSERR_NAT_END_OF_RULES                 (RTCS_ERROR_BASE|0xC0A)
#define RTCSERR_NAT_INVALID_PRIVATE_ADDRESS      (RTCS_ERROR_BASE|0xC0B)

/*
** IPCFG error codes
*/
#define IPCFG_OK                        RTCS_OK
#define RTCSERR_IPCFG_BUSY              (RTCS_ERROR_BASE|0xD01)
#define RTCSERR_IPCFG_DEVICE_NUMBER     (RTCS_ERROR_BASE|0xD02)
#define RTCSERR_IPCFG_INIT              (RTCS_ERROR_BASE|0xD03)
#define RTCSERR_IPCFG_BIND              (RTCS_ERROR_BASE|0xD04)
#define RTCSERR_IPCFG_NOT_INIT          (RTCS_ERROR_BASE|0xD05)


    /****************************************************************
    * Additional RTCS IPv6 error codes .
    *****************************************************************/
    #define RTCSERR_IP6_IF_NO_ADDRESS_SPACE      (RTCS_ERROR_BASE|0xE01)
    #define RTCSERR_ND6_CFG                      (RTCS_ERROR_BASE|0xE02)
    #define RTCSERR_IP_IS_DISABLED               (RTCS_ERROR_BASE|0xE03)


/*
** Protocol layer definitions.  These are used by RTCS_[non]fatal_error().
*/
#define ERROR_RTCS                  (0x1)
#define ERROR_SOCKET                (0x2)
#define ERROR_TCPIP                 (0x3)
#define ERROR_TCP                   (0x4)
#define ERROR_UDP                   (0x5)
#define ERROR_RPC                   (0x6)
#define ERROR_ECHO                  (0x7)
#define ERROR_EDS                   (0x8)
#define ERROR_TELNET                (0x9)
#define ERROR_DHCPSRV               (0xA)
#define ERROR_DNS                   (0xB)
#define ERROR_IGMP                  (0xC)




#define RTCS_BOOTFILE_LEN       (128)
#define RTCS_SNAME_LEN          (64)
#define RTCS_BOOTOPT_LEN        (64)

#define MEM_TYPE_RTCS_BASE                   ( (IO_RTCS_COMPONENT) << (MEM_TYPE_COMPONENT_SHIFT))
#define MEM_TYPE_RTCS_DATA                      (MEM_TYPE_RTCS_BASE+1)
#define MEM_TYPE_ARP_CFG                        (MEM_TYPE_RTCS_BASE+2)
#define MEM_TYPE_ARP_ENTRY                      (MEM_TYPE_RTCS_BASE+3)
#define MEM_TYPE_DHCP_CLNT_STRUCT               (MEM_TYPE_RTCS_BASE+4)
#define MEM_TYPE_DHCPSRV_OPTIONS_STRUCT         (MEM_TYPE_RTCS_BASE+5)
#define MEM_TYPE_FTPd_SESSION_PTR               (MEM_TYPE_RTCS_BASE+6)
#define MEM_TYPE_FTPd_DATA_BUFFER               (MEM_TYPE_RTCS_BASE+7)
#define MEM_TYPE_ICMP_CFG_STRUCT                (MEM_TYPE_RTCS_BASE+8)
#define MEM_TYPE_ICMP_DATA                      (MEM_TYPE_RTCS_BASE+9)
#define MEM_TYPE_IP_CFG_STRUCT                  (MEM_TYPE_RTCS_BASE+10)
#define MEM_TYPE_IP_DATA                        (MEM_TYPE_RTCS_BASE+11)
#define MEM_TYPE_ICB                            (MEM_TYPE_RTCS_BASE+12)
#define MEM_TYPE_IP_IF                          (MEM_TYPE_RTCS_BASE+13)
#define MEM_TYPE_SOCKET_CONFIG_STRUCT           (MEM_TYPE_RTCS_BASE+14)
#define MEM_TYPE_RTCS_PARTITION                 (MEM_TYPE_RTCS_BASE+15)
#define MEM_TYPE_IO_SOCKET                      (MEM_TYPE_RTCS_BASE+16)
#define MEM_TYPE_SOCKET_OWNER_STRUCT            (MEM_TYPE_RTCS_BASE+17)
#define MEM_TYPE_TCP_CFG_STRUCT                 (MEM_TYPE_RTCS_BASE+18)
#define MEM_TYPE_TCB                            (MEM_TYPE_RTCS_BASE+19)
#define MEM_TYPE_TCP_TX_WINDOW                  (MEM_TYPE_RTCS_BASE+20)
#define MEM_TYPE_TCP_RX_WINDOW                  (MEM_TYPE_RTCS_BASE+21)
#define MEM_TYPE_TCP_SEND_CLOCK                 (MEM_TYPE_RTCS_BASE+22)
#define MEM_TYPE_UDP_CFG_STRUCT                 (MEM_TYPE_RTCS_BASE+23)
#define MEM_TYPE_UCB_STRUCT                     (MEM_TYPE_RTCS_BASE+24)
#define MEM_TYPE_UDP_BUFFER                     (MEM_TYPE_RTCS_BASE+25)
#define MEM_TYPE_IO_TELNET                      (MEM_TYPE_RTCS_BASE+26)
#define MEM_TYPE_SbufNode                       (MEM_TYPE_RTCS_BASE+27)
#define MEM_TYPE_Rchunk                         (MEM_TYPE_RTCS_BASE+28)
#define MEM_TYPE_FTPc_CONTEXT_STRUCT            (MEM_TYPE_RTCS_BASE+29)
#define MEM_TYPE_DHCPSRV_ADDR_STRUCT            (MEM_TYPE_RTCS_BASE+30)
#define MEM_TYPE_DHCPSRV_CID                    (MEM_TYPE_RTCS_BASE+31)
#define MEM_TYPE_DNS_CONTROL_STRUCT             (MEM_TYPE_RTCS_BASE+32)
#define MEM_TYPE_DNS_UDP_MESSAGE                (MEM_TYPE_RTCS_BASE+33)
#define MEM_TYPE_FTPc_RX_BUFFER                 (MEM_TYPE_RTCS_BASE+34)
#define MEM_TYPE_FTPc_COMMAND_BUFFER            (MEM_TYPE_RTCS_BASE+35)
#define MEM_TYPE_HOSTENT_STRUCT                 (MEM_TYPE_RTCS_BASE+36)
#define MEM_TYPE_TFTP_PACKET                    (MEM_TYPE_RTCS_BASE+37)
#define MEM_TYPE_TFTP_TRANS_STRUCT              (MEM_TYPE_RTCS_BASE+38)

#define MEM_TYPE_HTTPD_STRUCT                   (MEM_TYPE_RTCS_BASE+40)
#define MEM_TYPE_HTTPD_PARAMS                   (MEM_TYPE_RTCS_BASE+41)
#define MEM_TYPE_HTTPD_SESSION_STRUCT           (MEM_TYPE_RTCS_BASE+42)
#define MEM_TYPE_HTTPD_URI                      (MEM_TYPE_RTCS_BASE+43)
#define MEM_TYPE_HTTPD_AUTH                     (MEM_TYPE_RTCS_BASE+44)
#define MEM_TYPE_HTTPD_LINE                     (MEM_TYPE_RTCS_BASE+45)


#define MEM_TYPE_ICMP6_CFG_STRUCT               (MEM_TYPE_RTCS_BASE+50)
#define MEM_TYPE_ND6_CFG                        (MEM_TYPE_RTCS_BASE+51)
#define MEM_TYPE_IP6_CFG_STRUCT                 (MEM_TYPE_RTCS_BASE+52)


/* Helpful macros.*/
#define IPBYTES(a)            (((a)>>24)&0xFF),(((a)>>16)&0xFF),(((a)>> 8)&0xFF),((a)&0xFF)
#define IPADDR(a,b,c,d)       (((uint_32)(a)&0xFF)<<24)|(((uint_32)(b)&0xFF)<<16)|(((uint_32)(c)&0xFF)<<8)|((uint_32)(d)&0xFF)

/*
** Configurable globals
*/
extern uint_32 _RTCSTASK_priority;     /* TCP/IP task priority  */
extern uint_32 _RTCSTASK_stacksize;    /* additional stack size */

extern uint_32 _RTCSQUEUE_base;        /* RTCS queue numbers to use  */
extern uint_32 _RTCSPCB_init;           /* maximum RTCSPCBs available */
extern uint_32 _RTCSPCB_grow;           /* maximum RTCSPCBs available */
extern uint_32 _RTCSPCB_max;           /* maximum RTCSPCBs available */

extern boolean _IP_forward;            /* IP forwarding */

extern boolean _TCP_bypass_rx;         /* TCP checksum bypass (recv) */
extern boolean _TCP_bypass_tx;         /* TCP checksum bypass (send) */
extern uint_32 _TCP_rto_min;           /* minimum TCP resend timeout */
extern boolean _RTCS_initialized;      /* RTCS initialized */
extern uint_32 _UDP_max_queue_size;    /* maximum length of UDP queue */
extern boolean _DHCP_broadcast;        /* DHCP broadcast or unicast offers */

extern _mem_pool_id _RTCS_mem_pool;

extern uint_32 _RTCS_msgpool_init;
extern uint_32 _RTCS_msgpool_grow;
extern uint_32 _RTCS_msgpool_max;

extern uint_32 _RTCS_socket_part_init;
extern uint_32 _RTCS_socket_part_grow;
extern uint_32 _RTCS_socket_part_max;

extern uint_32 _RTCS_dhcp_term_timeout;

/*
** Type definitions
*/

/*
** IP address definition
*/
typedef uint_32 _ip_address;

typedef struct in_addr {
   _ip_address s_addr;
} in_addr;

/*
** IP multicast group
*/
typedef struct ip_mreq {
   in_addr  imr_multiaddr;
   in_addr  imr_interface;
} ip_mreq;

/*
** Socket Address Structure
*/
typedef struct sockaddr_in {
   uint_16  sin_family;
   uint_16  sin_port;
   in_addr  sin_addr;
} sockaddr_in;


#if RTCSCFG_ENABLE_IP6
    typedef struct sockaddr
    {
        uint_16		sa_family;
        char 		sa_data[22];
    } sockaddr;
#else
    #if RTCSCFG_ENABLE_IP4
        #define sockaddr   sockaddr_in
        #define sa_family   sin_family
    #endif
#endif

/****************************************************************
* IPv6 address definition (BSD-like).
*****************************************************************/
typedef struct in6_addr
{
    union
    {
        uint_8      __u6_addr8[16];
        uint_16     __u6_addr16[8];
        uint_32 	__u6_addr32[4];
    } __u6_addr;			/* 128-bit IP6 address */
}in6_addr;
#define s6_addr   __u6_addr.__u6_addr8
/* Not RFC */
#define s6_addr8  __u6_addr.__u6_addr8
#define s6_addr16 __u6_addr.__u6_addr16
#define s6_addr32 __u6_addr.__u6_addr32

/****************************************************************
* Socket address for IPv6 (BSD-like).
*****************************************************************/
typedef struct sockaddr_in6
{
    uint_16     sin6_family;    /* AF_INET6 */
    uint_16     sin6_port;      /* Transport layer port # */
    in6_addr	sin6_addr;      /* IP6 address */
    uint_32     sin6_scope_id;  /* scope zone index */
}sockaddr_in6;

/****************************************************************
*
* Helpful macros.
*
*****************************************************************/
#define IN6ADDR(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16) \
    	    { (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8),       \
            (a9), (a10), (a11), (a12), (a13), (a14), (a15), (a16) }

/*
 * Construct an IPv6 address from eight 16-bit words.
*/
#define in6addr(a, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)    \
                         {                                                                  \
                            (a)->s6_addr[0] = a0;                                           \
                            (a)->s6_addr[1] = a1;                                           \
                            (a)->s6_addr[2] = a2;                                           \
                            (a)->s6_addr[3] = a3;                                           \
                            (a)->s6_addr[4] = a4;                                           \
                            (a)->s6_addr[5] = a5;                                           \
                            (a)->s6_addr[6] = a6;                                           \
                            (a)->s6_addr[7] = a7;                                           \
                            (a)->s6_addr[8] = a8;                                           \
                            (a)->s6_addr[9] = a9;                                           \
                            (a)->s6_addr[10] = a10;                                         \
                            (a)->s6_addr[11] = a11;                                         \
                            (a)->s6_addr[12] = a12;                                         \
                            (a)->s6_addr[13] = a13;                                         \
                            (a)->s6_addr[14] = a14;                                         \
                            (a)->s6_addr[15] = a15;                                         \
                        }

/*
* Definition of some useful macros to handle IP6 addresses (BSD-like)
*/
#define IN6ADDR_ANY_INIT                                    \
    {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}}
#define IN6ADDR_LOOPBACK_INIT                               \
    {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}}
#define IN6ADDR_NODELOCAL_ALLNODES_INIT                     \
    {{{ 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}}
#define IN6ADDR_INTFACELOCAL_ALLNODES_INIT                  \
    {{{ 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}}
#define IN6ADDR_LINKLOCAL_ALLNODES_INIT                     \
    {{{ 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}}
#define IN6ADDR_LINKLOCAL_ALLROUTERS_INIT                   \
    {{{ 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 }}}
#define IN6ADDR_LINKLOCAL_ALLV2ROUTERS_INIT                 \
    {{{ 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16 }}}

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;
extern const struct in6_addr in6addr_nodelocal_allnodes;
extern const struct in6_addr in6addr_linklocal_allnodes;
extern const struct in6_addr in6addr_linklocal_allrouters;
extern const struct in6_addr in6addr_linklocal_allv2routers;

/*
* Equality (BSD-like)
*/
#define IN6_ARE_ADDR_EQUAL(a, b)			\
        (memcmp(&(a)->s6_addr[0], &(b)->s6_addr[0], sizeof(struct in6_addr)) == 0)

/*
* Copying address
*/
extern void IN6_ADDR_COPY(in6_addr *from_addr, in6_addr *to_addr);


/*
 * Unspecified (BSD-like)
 */
#define IN6_IS_ADDR_UNSPECIFIED(a)	                                \
    ((*(const uint_32 *)(const void *)(&(a)->s6_addr[0]) == 0) &&	\
    (*(const uint_32 *)(const void *)(&(a)->s6_addr[4]) == 0) &&	\
    (*(const uint_32 *)(const void *)(&(a)->s6_addr[8]) == 0) &&	\
    (*(const uint_32 *)(const void *)(&(a)->s6_addr[12]) == 0))

/*
 * Loopback (BSD-like)
 */
#define IN6_IS_ADDR_LOOPBACK(a)	                                	\
    ((*(const uint_32 *)(const void *)(&(a)->s6_addr[0]) == 0) &&	\
    (*(const uint_32 *)(const void *)(&(a)->s6_addr[4]) == 0) &&	\
    (*(const uint_32 *)(const void *)(&(a)->s6_addr[8]) == 0) &&	\
    (*(const uint_32 *)(const void *)(&(a)->s6_addr[12]) == ntohl(1)))

/*
 * Multicast (BSD-like)
 */
#define IN6_IS_ADDR_MULTICAST(a)	((a)->s6_addr[0] == 0xff)

/*
 * Unicast Scope (BSD-like)
 * Note that we must check topmost 10 bits only, not 16 bits (see RFC2373).
 */
#define IN6_IS_ADDR_LINKLOCAL(a)	\
    	(((a)->s6_addr[0] == 0xfe) && (((a)->s6_addr[1] & 0xc0) == 0x80))
#define IN6_IS_ADDR_SITELOCAL(a)	\
    	(((a)->s6_addr[0] == 0xfe) && (((a)->s6_addr[1] & 0xc0) == 0xc0))

/*
 * How the address was acquired.
 */
typedef enum
{
    IP6_ADDR_TYPE_MANUAL = 0,           /* Manually.*/
    IP6_ADDR_TYPE_AUTOCONFIGURABLE = 1  /* Autoconfigurable address. */
} ip6_if_addr_type;

/**********************************************************************
 * Link-layer address.
 * For example, Ethernet interafce uses the address with size set to 6.
***********************************************************************/
typedef uchar ll_addr_t[16];
/*
 * Copying Link-layer address.
 */
#define LL_ADDR_COPY(from_addr, to_addr, ll_size)   \
        (_mem_copy(&from_addr[0], &to_addr[0], ll_size))

/*
 * Equality.
 */
#define LL_ADDR_ARE_EQUAL(a, b, size)               \
        (memcmp(&a[0], &b[0], size) == 0)


/*
** The protocol table
*/
extern uint_32 (_CODE_PTR_ const RTCS_protocol_table[])(void);


/*
** Protocol statistics
*/


/*
** Common statistics kept across all protocols.
*/
typedef struct rtcs_stats_struct {
   uint_32  ST_RX_TOTAL;
   uint_32  ST_RX_MISSED;
   uint_32  ST_RX_DISCARDED;
   uint_32  ST_RX_ERRORS;

   uint_32  ST_TX_TOTAL;
   uint_32  ST_TX_MISSED;
   uint_32  ST_TX_DISCARDED;
   uint_32  ST_TX_ERRORS;
} RTCS_STATS_STRUCT, _PTR_ RTCS_STATS_STRUCT_PTR;

/*
** The error structure embedded in statistics structures
*/
typedef struct rtcs_error_struct {
   uint_32     ERROR;
   uint_32     PARM;
   _task_id    TASK_ID;
   uint_32     TASKCODE;
   pointer     MEMPTR;
   boolean     STACK;
} RTCS_ERROR_STRUCT, _PTR_ RTCS_ERROR_STRUCT_PTR;



/*
** In the structures below, the following four fields are common:
**
** [RT]X_TOTAL      is the total number of packets sent/received
** [RT]X_MISSED     is the number of packets discarded due to lack of resources
** [RT]X_DISCARDED  is the number of packets discarded for all other reasons
** [RT]X_ERRORS     is the number of times an internal error occurred
**
** Internal errors are errors that should _never_ happen.  However,
** if one does occur, information is recorded in ERR_[RT]X.
*/
#if RTCSCFG_ENABLE_IPIF_STATS
typedef struct ipif_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_OCTETS;           /* total bytes received       */
   uint_32  ST_RX_UNICAST;          /* unicast packets received   */
   uint_32  ST_RX_MULTICAST;        /* multicast packets received */
   uint_32  ST_RX_BROADCAST;        /* broadcast packets received */

   uint_32  ST_TX_OCTETS;           /* total bytes sent           */
   uint_32  ST_TX_UNICAST;          /* unicast packets sent       */
   uint_32  ST_TX_MULTICAST;        /* multicast packets sent     */
   uint_32  ST_TX_BROADCAST;        /* broadcast packets sent     */

} IPIF_STATS, _PTR_ IPIF_STATS_PTR;
#endif

#if RTCSCFG_ENABLE_ARP_STATS
typedef struct arp_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_REQUESTS;         /* valid ARP requests received */
   uint_32  ST_RX_REPLIES;          /* valid ARP replies received  */

   uint_32  ST_TX_REQUESTS;         /* ARP requests sent           */
   uint_32  ST_TX_REPLIES;          /* ARP replies received        */

   uint_32  ST_ALLOCS_FAILED;       /* ARP_alloc returned NULL     */
   uint_32  ST_CACHE_HITS;          /* ARP cache hits              */
   uint_32  ST_CACHE_MISSES;        /* ARP cache misses            */
   uint_32  ST_PKT_DISCARDS;        /* data packets discarded due  */
                                    /*    to missing ARP entry     */

} ARP_STATS, _PTR_ ARP_STATS_PTR;
#endif

#if RTCSCFG_ENABLE_IP_STATS
typedef struct ip_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_HDR_ERRORS;       /* Discarded -- error in IP header    */
   uint_32  ST_RX_ADDR_ERRORS;      /* Discarded -- illegal destination   */
   uint_32  ST_RX_NO_PROTO;         /* Discarded -- unrecognized protocol */
   uint_32  ST_RX_DELIVERED;        /* Datagrams delivered to upper layer */
   uint_32  ST_RX_FORWARDED;        /* Datagrams forwarded                */

   /* These are included in ST_RX_DISCARDED and ST_RX_HDR_ERRORS */
   uint_32  ST_RX_BAD_VERSION;      /* Datagrams with version != 4        */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Datagrams with invalid checksum    */
   uint_32  ST_RX_BAD_SOURCE;       /* Datagrams with invalid src address */
   uint_32  ST_RX_SMALL_HDR;        /* Datagrams with header too small    */
   uint_32  ST_RX_SMALL_DGRAM;      /* Datagrams smaller than header      */
   uint_32  ST_RX_SMALL_PKT;        /* Datagrams larger than frame        */
   uint_32  ST_RX_TTL_EXCEEDED;     /* Datagrams to route with TTL = 0    */

   uint_32  ST_RX_FRAG_RECVD;       /* Number of received IP fragments    */
   uint_32  ST_RX_FRAG_REASMD;      /* Number of reassembled datagrams    */
   uint_32  ST_RX_FRAG_DISCARDED;   /* Number of discarded fragments      */

   uint_32  ST_TX_FRAG_SENT;        /* Number of sent fragments           */
   uint_32  ST_TX_FRAG_FRAGD;       /* Number of fragmented datagrams     */
   uint_32  ST_TX_FRAG_DISCARDED;   /* Number of fragmentation failures   */

} IP_STATS, _PTR_ IP_STATS_PTR;
#endif

#if RTCSCFG_ENABLE_IP6_STATS
typedef struct ip6_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_HDR_ERRORS;       /* Discarded -- error in IP header    */
   uint_32  ST_RX_ADDR_ERRORS;      /* Discarded -- illegal destination   */
   uint_32  ST_RX_NO_PROTO;         /* Discarded -- unrecognized protocol */
   uint_32  ST_RX_DELIVERED;        /* Datagrams delivered to upper layer */
   uint_32  ST_RX_FORWARDED;        /* Datagrams forwarded                */

   /* These are included in ST_RX_DISCARDED and ST_RX_HDR_ERRORS */
   uint_32  ST_RX_BAD_VERSION;      /* Datagrams with version != 4        */
   uint_32  ST_RX_BAD_SOURCE;       /* Datagrams with invalid src address */
   uint_32  ST_RX_SMALL_PKT;        /* Datagrams larger than frame        */
   uint_32  ST_RX_TTL_EXCEEDED;     /* Datagrams to route with TTL = 0    */

   uint_32  ST_RX_FRAG_RECVD;       /* Number of received IP fragments    */
   uint_32  ST_RX_FRAG_REASMD;      /* Number of reassembled datagrams    */
   uint_32  ST_RX_FRAG_DISCARDED;   /* Number of discarded fragments      */

   uint_32  ST_TX_FRAG_SENT;        /* Number of sent fragments           */
   uint_32  ST_TX_FRAG_FRAGD;       /* Number of fragmented datagrams     */
   uint_32  ST_TX_FRAG_DISCARDED;   /* Number of fragmentation failures   */

} IP6_STATS, _PTR_ IP6_STATS_PTR;
#endif

#if RTCSCFG_ENABLE_ICMP_STATS
typedef struct icmp_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   /* These are included in ST_RX_DISCARDED */
   uint_32  ST_RX_BAD_CODE;         /* Datagrams with unrecognized code     */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Datagrams with invalid checksum      */
   uint_32  ST_RX_SMALL_DGRAM;      /* Datagrams smaller than header        */
   uint_32  ST_RX_RD_NOTGATE;       /* Redirects received from non-gateway  */

   /* Statistics on each ICMP type */
   uint_32  ST_RX_DESTUNREACH;      /* Received Destination Unreachables    */
   uint_32  ST_RX_TIMEEXCEED;       /* Received Time Exceededs              */
   uint_32  ST_RX_PARMPROB;         /* Received Parameter Problems          */
   uint_32  ST_RX_SRCQUENCH;        /* Received Source Quenches             */

   uint_32  ST_RX_REDIRECT;         /* Received Redirects                   */
   uint_32  ST_RX_ECHO_REQ;         /* Received Echo Requests               */
   uint_32  ST_RX_ECHO_REPLY;       /* Received Echo Replys                 */
   uint_32  ST_RX_TIME_REQ;         /* Received Timestamp Requests          */

   uint_32  ST_RX_TIME_REPLY;       /* Received Timestamp Replys            */
   uint_32  ST_RX_INFO_REQ;         /* Received Information Requests        */
   uint_32  ST_RX_INFO_REPLY;       /* Received Information Replys          */
   uint_32  ST_RX_OTHER;            /* All other types                      */

   uint_32  ST_TX_DESTUNREACH;      /* Transmitted Destination Unreachables */
   uint_32  ST_TX_TIMEEXCEED;       /* Transmitted Time Exceededs           */
   uint_32  ST_TX_PARMPROB;         /* Transmitted Parameter Problems       */
   uint_32  ST_TX_SRCQUENCH;        /* Transmitted Source Quenches          */

   uint_32  ST_TX_REDIRECT;         /* Transmitted Redirects                */
   uint_32  ST_TX_ECHO_REQ;         /* Transmitted Echo Requests            */
   uint_32  ST_TX_ECHO_REPLY;       /* Transmitted Echo Replys              */
   uint_32  ST_TX_TIME_REQ;         /* Transmitted Timestamp Requests       */

   uint_32  ST_TX_TIME_REPLY;       /* Transmitted Timestamp Replys         */
   uint_32  ST_TX_INFO_REQ;         /* Transmitted Information Requests     */
   uint_32  ST_TX_INFO_REPLY;       /* Transmitted Information Replys       */
   uint_32  ST_TX_OTHER;            /* All other types                      */

} ICMP_STATS, _PTR_ ICMP_STATS_PTR;
#endif

#if RTCSCFG_ENABLE_ICMP6_STATS
typedef struct icmp6_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   /* These are included in ST_RX_DISCARDED */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Datagrams with invalid checksum      */
   uint_32  ST_RX_SMALL_DGRAM;      /* Datagrams smaller than header        */

   /* Statistics on each ICMP type */
   uint_32  ST_RX_DESTUNREACH;      /* Received Destination Unreachables    */
   uint_32  ST_RX_TIMEEXCEED;       /* Received Time Exceededs              */
   uint_32  ST_RX_PARMPROB;         /* Received Parameter Problems          */
   uint_32  ST_RX_REDIRECT;         /* Received Redirects                   */
   uint_32  ST_RX_ECHO_REQ;         /* Received Echo Requests               */
   uint_32  ST_RX_ECHO_REPLY;       /* Received Echo Replys                 */
   uint_32  ST_RX_OTHER;            /* All other types                      */
   uint_32  ST_TX_DESTUNREACH;      /* Transmitted Destination Unreachables */
   uint_32  ST_TX_TIMEEXCEED;       /* Transmitted Time Exceededs           */
   uint_32  ST_TX_PARMPROB;         /* Transmitted Parameter Problems       */
   uint_32  ST_TX_REDIRECT;         /* Transmitted Redirects                */
   uint_32  ST_TX_ECHO_REQ;         /* Transmitted Echo Requests            */
   uint_32  ST_TX_ECHO_REPLY;       /* Transmitted Echo Replys              */
   uint_32  ST_TX_OTHER;            /* All other types                      */

} ICMP6_STATS, _PTR_ ICMP6_STATS_PTR;
#endif

#if RTCSCFG_ENABLE_IGMP_STATS
typedef struct igmp_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32  ST_RX_BAD_TYPE;         /* Datagrams with unrecognized code     */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Datagrams with invalid checksum      */
   uint_32  ST_RX_SMALL_DGRAM;      /* Datagrams smaller than header        */

   uint_32  ST_RX_QUERY;            /* Received Queries                     */
   uint_32  ST_RX_REPORT;           /* Received Reports                     */

   uint_32  ST_TX_QUERY;            /* Transmitted Queries                  */
   uint_32  ST_TX_REPORT;           /* Transmitted Reports                  */

} IGMP_STATS, _PTR_ IGMP_STATS_PTR;
#endif

#if RTCSCFG_ENABLE_UDP_STATS
typedef struct udp_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   /* These are all included in ST_RX_DISCARDED */
   uint_32  ST_RX_BAD_PORT;         /* Datagrams with dest port zero   */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Datagrams with invalid checksum */
   uint_32  ST_RX_SMALL_DGRAM;      /* Datagrams smaller than header   */
   uint_32  ST_RX_SMALL_PKT;        /* Datagrams larger than frame     */
   uint_32  ST_RX_NO_PORT;          /* Datagrams for a closed port     */

} UDP_STATS, _PTR_ UDP_STATS_PTR;
#endif

#if RTCSCFG_ENABLE_TCP_STATS
typedef struct tcp_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   /* These are all included in ST_RX_DISCARDED */
   uint_32  ST_RX_BAD_PORT;         /* Segments with dest port zero   */
   uint_32  ST_RX_BAD_CHECKSUM;     /* Segments with invalid checksum */
   uint_32  ST_RX_BAD_OPTION;       /* Segments with invalid options  */
   uint_32  ST_RX_BAD_SOURCE;       /* Segments with invalid source   */
   uint_32  ST_RX_SMALL_HDR;        /* Segments with header too small */
   uint_32  ST_RX_SMALL_DGRAM;      /* Segments smaller than header   */
   uint_32  ST_RX_SMALL_PKT;        /* Segments larger than frame     */
   uint_32  ST_RX_BAD_ACK;          /* Received ack for unsent data   */
   uint_32  ST_RX_BAD_DATA;         /* Received data outside window   */
   uint_32  ST_RX_LATE_DATA;        /* Received data after close      */
   uint_32  ST_RX_OPT_MSS;          /* Segments with MSS option set   */
   uint_32  ST_RX_OPT_OTHER;        /* Segments with other options    */

   uint_32  ST_RX_DATA;             /* Data segments received         */
   uint_32  ST_RX_DATA_DUP;         /* Duplicate data received        */
   uint_32  ST_RX_ACK;              /* Acks received                  */
   uint_32  ST_RX_ACK_DUP;          /* Duplicate acks received        */
   uint_32  ST_RX_RESET;            /* RST segments received          */
   uint_32  ST_RX_PROBE;            /* Window probes received         */
   uint_32  ST_RX_WINDOW;           /* Window updates received        */

   uint_32  ST_RX_SYN_EXPECTED;     /* Expected SYN, not received     */
   uint_32  ST_RX_ACK_EXPECTED;     /* Expected ACK, not received     */
   uint_32  ST_RX_SYN_NOT_EXPECTED; /* Received SYN, not expected     */
   uint_32  ST_RX_MULTICASTS;       /* Multicast packets              */


   uint_32  ST_TX_DATA;             /* Data segments sent             */
   uint_32  ST_TX_DATA_DUP;         /* Data segments retransmitted    */
   uint_32  ST_TX_ACK;              /* Ack-only segments sent         */
   uint_32  ST_TX_ACK_DELAYED;      /* Delayed acks sent              */
   uint_32  ST_TX_RESET;            /* RST segments sent              */
   uint_32  ST_TX_PROBE;            /* Window probes sent             */
   uint_32  ST_TX_WINDOW;           /* Window updates sent            */

   uint_32  ST_CONN_ACTIVE;         /* Active opens                   */
   uint_32  ST_CONN_PASSIVE;        /* Passive opens                  */
   uint_32  ST_CONN_OPEN;           /* Established connections        */
   uint_32  ST_CONN_CLOSED;         /* Graceful shutdowns             */
   uint_32  ST_CONN_RESET;          /* Ungraceful shutdowns           */
   uint_32  ST_CONN_FAILED;         /* Failed opens                   */
   uint_32  ST_CONN_ABORTS;         /* Aborts                         */

} TCP_STATS, _PTR_ TCP_STATS_PTR;
#endif

#if RTCSCFG_ENABLE_RIP_STATS
typedef struct rip_stats {
   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

} RIP_STATS, _PTR_ RIP_STATS_PTR;
#endif


/*
** Type definitions for RTCS_if_XXX()
*/

typedef pointer _rtcs_if_handle;
struct ip_if;
struct rtcspcb;

struct in6_addr; //Forward declaration.

typedef struct  rtcs_if_struct {
   uint_32 (_CODE_PTR_  OPEN) (struct ip_if _PTR_);
   uint_32 (_CODE_PTR_  CLOSE)(struct ip_if _PTR_);
#if RTCSCFG_ENABLE_IP4
   uint_32 (_CODE_PTR_  SEND) (struct ip_if _PTR_, struct rtcspcb _PTR_, _ip_address, _ip_address, pointer);
#if RTCSCFG_ENABLE_IGMP && BSPCFG_ENABLE_ENET_MULTICAST
   uint_32 (_CODE_PTR_  JOIN) (struct ip_if _PTR_, _ip_address);
   uint_32 (_CODE_PTR_  LEAVE)(struct ip_if _PTR_, _ip_address);
#endif /* RTCSCFG_ENABLE_IGMP */
#endif /* RTCSCFG_ENABLE_IP4 */

#if RTCSCFG_ENABLE_IP6
   uint_32 (_CODE_PTR_  SEND6) (struct ip_if _PTR_, struct rtcspcb _PTR_, struct in6_addr _PTR_, struct in6_addr _PTR_);
   uint_32 (_CODE_PTR_  JOIN6) (struct ip_if _PTR_, const struct in6_addr _PTR_);
   uint_32 (_CODE_PTR_  LEAVE6)(struct ip_if _PTR_, const struct in6_addr _PTR_);
#endif

} RTCS_IF_STRUCT, _PTR_ RTCS_IF_STRUCT_PTR;

extern const RTCS_IF_STRUCT_PTR  RTCS_IF_LOCALHOST;
extern const RTCS_IF_STRUCT_PTR  RTCS_IF_ENET;
extern const RTCS_IF_STRUCT_PTR  RTCS_IF_PPP;



/*
** Required when booting with BOOTP
*/
typedef struct bootp_data_struct {
   _ip_address SADDR;
#if RTCSCFG_BOOTP_RETURN_YIADDR
   _ip_address CLIENTADDR;
#endif
   uchar       SNAME[RTCS_SNAME_LEN];
   uchar       BOOTFILE[RTCS_BOOTFILE_LEN];
   uchar       OPTIONS[RTCS_BOOTOPT_LEN];
} BOOTP_DATA_STRUCT, _PTR_ BOOTP_DATA_STRUCT_PTR;

/*
** Required when using PPP
*/
typedef struct ipcp_data_struct {
   void (_CODE_PTR_  IP_UP)   (pointer);
   void (_CODE_PTR_  IP_DOWN) (pointer);
   pointer           IP_PARAM;

   unsigned ACCEPT_LOCAL_ADDR  : 1;
   unsigned ACCEPT_REMOTE_ADDR : 1;
   unsigned DEFAULT_NETMASK    : 1; /* obsolete */
   unsigned DEFAULT_ROUTE      : 1;
   unsigned NEG_LOCAL_DNS      : 1;
   unsigned NEG_REMOTE_DNS     : 1;
   unsigned ACCEPT_LOCAL_DNS   : 1; /* ignored if NEG_LOCAL_DNS  == 0 */
   unsigned ACCEPT_REMOTE_DNS  : 1; /* ignored if NEG_REMOTE_DNS == 0 */
   unsigned                    : 0;

   _ip_address LOCAL_ADDR;
   _ip_address REMOTE_ADDR;
   _ip_address NETMASK;     /* obsolete */
   _ip_address LOCAL_DNS;   /* ignored if NEG_LOCAL_DNS   == 0 */
   _ip_address REMOTE_DNS;  /* ignored if NEG_REMOTE_DNS  == 0 */

} IPCP_DATA_STRUCT, _PTR_ IPCP_DATA_STRUCT_PTR;


/*
** Type definitions for socket()
*/

typedef struct rtcs_socket_call_struct {
   uint_32 (_CODE_PTR_ SOCK_SOCKET)      (uint_32);
   uint_32 (_CODE_PTR_ SOCK_BIND)        (uint_32, const sockaddr _PTR_ , uint_16);
   uint_32 (_CODE_PTR_ SOCK_CONNECT)     (uint_32, const sockaddr _PTR_, uint_16);
   uint_32 (_CODE_PTR_ SOCK_LISTEN)      (uint_32, uint_32);
   uint_32 (_CODE_PTR_ SOCK_ACCEPT)      (uint_32, sockaddr _PTR_, uint_16 _PTR_);
   uint_32 (_CODE_PTR_ SOCK_GETSOCKNAME) (uint_32, sockaddr _PTR_, uint_16 _PTR_);
   uint_32 (_CODE_PTR_ SOCK_GETPEERNAME) (uint_32, sockaddr _PTR_, uint_16 _PTR_);
    int_32 (_CODE_PTR_ SOCK_RECV)        (uint_32, pointer, uint_32, uint_32);
    int_32 (_CODE_PTR_ SOCK_RECVFROM)    (uint_32, pointer, uint_32, uint_32, sockaddr _PTR_, uint_16 _PTR_);
    int_32 (_CODE_PTR_ SOCK_RECVMSG)     (uint_32, pointer, uint_32);
    int_32 (_CODE_PTR_ SOCK_SEND)        (uint_32, pointer, uint_32, uint_32);
    int_32 (_CODE_PTR_ SOCK_SENDTO)      (uint_32, pointer, uint_32, uint_32, sockaddr _PTR_, uint_16);
    int_32 (_CODE_PTR_ SOCK_SENDMSG)     (uint_32, pointer, uint_32);
   uint_32 (_CODE_PTR_ SOCK_SOCKATMARK)  (uint_32);
   uint_32 (_CODE_PTR_ SOCK_SHUTDOWN)    (uint_32, uint_32);
} RTCS_SOCKET_CALL_STRUCT, _PTR_ RTCS_SOCKET_CALL_STRUCT_PTR;

extern const RTCS_SOCKET_CALL_STRUCT  SOCK_DGRAM_CALL;
extern const RTCS_SOCKET_CALL_STRUCT  SOCK_STREAM_CALL;

/*
** Type definitions for [gs]etsockopt()
*/

typedef struct rtcs_sockopt_call_struct  {
   uint_32 (_CODE_PTR_ SOCK_GETSOCKOPT)  (uint_32, uint_32, uint_32, pointer, uint_32 _PTR_);
   uint_32 (_CODE_PTR_ SOCK_SETSOCKOPT)  (uint_32, uint_32, uint_32, pointer, uint_32);
} RTCS_SOCKOPT_CALL_STRUCT, _PTR_ RTCS_SOCKOPT_CALL_STRUCT_PTR;

extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_SOCKET_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_IP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_UDP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_TCP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_IGMP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_LINK_CALL;

/*
** The socket state structure
*/

typedef struct rtcs_linkopt_struct {
   unsigned    OPT_8023 : 1;
   unsigned    OPT_PRIO : 1;
   unsigned             : 0;
   _ip_address DEST;
   uchar       TTL;
   uchar       TOS;
   uchar       RESERVED[2];
#if RTCSCFG_LINKOPT_8021Q_PRIO
   uint_32     PRIO;
#endif
} RTCS_LINKOPT_STRUCT, _PTR_ RTCS_LINKOPT_STRUCT_PTR;

typedef struct socket_owner_struct {
   struct socket_owner_struct  _PTR_   NEXT;
   _rtcs_taskid                        TASK[SOCKET_NUMOWNERS];
} SOCKET_OWNER_STRUCT, _PTR_ SOCKET_OWNER_STRUCT_PTR;

struct mc_member;
struct ucb_struct;
struct tcb_struct;

typedef struct socket_struct {

   struct   socket_struct _PTR_  NEXT;
   struct   socket_struct _PTR_  PREV;

   uint_32                       VALID;

   uint_16                       LOCAL_PORT;          /* not used */
   uint_16                       REMOTE_PORT;         /* not used */
   _ip_address                   LOCAL_IPADDRESS;     /* not used */
   _ip_address                   REMOTE_IPADDRESS;    /* not used */

   /*TBD later we need make only one variable for IPv4 adn IPv6 */
#if RTCSCFG_ENABLE_IP6
   in6_addr                      LOCAL_IPV6ADDRESS;  /*it will be used for*/
   in6_addr                      REMOTE_IPV6ADDRESS; /*IPv6 version of socket */
#endif
   uint_16                       STATE;
   uint_16                       AF;
   RTCS_SOCKET_CALL_STRUCT_PTR   PROTOCOL;

   /*
   ** Socket options.
   **
   **  The order of these options must match the order of the option ids
   **  defined above, and CONNECT_TIMEOUT must be the first one.
   */
   uint_32                       CONNECT_TIMEOUT;
   uint_32                       RETRANSMISSION_TIMEOUT;
   uint_32                       SEND_TIMEOUT;
   uint_32                       RECEIVE_TIMEOUT;
   uint_32                       RECEIVE_PUSH;
   uint_32                       SEND_NOWAIT;
   uint_32                       SEND_WAIT;
   uint_32                       SEND_PUSH;
   uint_32                       RECEIVE_NOWAIT;
   boolean                       CHECKSUM_BYPASS;
   boolean                       ACTIVE;
   int_32                        TBSIZE;
   int_32                        RBSIZE;
   int_32                        MAXRTO;
   uint_32                       MAXRCV_WND;
   int_32                        KEEPALIVE;
   boolean                       NOWAIT;
   boolean                       NO_NAGLE_ALGORITHM;
   boolean                       NOSWRBUF;
   void _CODE_PTR_               CALL_BACK; /* obsolete */
   uint_32                       TYPE_MIRROR;
   void (_CODE_PTR_              APPLICATION_CALLBACK)(int_32);
   uint_32                       TIMEWAIT_TIMEOUT;
   boolean                       TCPSECUREDRAFT_0;
   uint_32                       DELAY_ACK;     /* last option */

   struct tcb_struct _PTR_       TCB_PTR;
   struct ucb_struct _PTR_       UCB_PTR;

   uint_32                       ERROR_CODE;
#if RTCSCFG_SOCKET_OWNERSHIP
   SOCKET_OWNER_STRUCT           OWNERS;
#endif

   struct mc_member _PTR_ _PTR_  MCB_PTR;
   uint_32 (_CODE_PTR_ _PTR_     IGMP_LEAVEALL)(struct mc_member _PTR_ _PTR_);

   struct {
      RTCS_LINKOPT_STRUCT        RX;
      RTCS_LINKOPT_STRUCT        TX;
   }                             LINK_OPTIONS;
} SOCKET_STRUCT, _PTR_ SOCKET_STRUCT_PTR;


/*
** Type definitions for the Telnet server
*/

typedef struct rtcs_task {
   char_ptr          NAME;
   uint_32           PRIORITY;
   uint_32           STACKSIZE;
   void (_CODE_PTR_  START)(pointer);
   pointer           ARG;
} RTCS_TASK, _PTR_ RTCS_TASK_PTR;


/*
** Type definitions for RTCS_load()
*/

/*
** File Transfer information structure
*/
typedef struct ft_call_struct {
   uint_32     (_CODE_PTR_ OPEN)    (pointer ft_data);
   uchar_ptr   (_CODE_PTR_ READ)    (uint_32_ptr size);
   boolean     (_CODE_PTR_ EOFT)    (void);
   uint_32     (_CODE_PTR_ CLOSE)   (void);
} FT_CALL_STRUCT, _PTR_ FT_CALL_STRUCT_PTR;

extern const FT_CALL_STRUCT_PTR FT_TFTP;
extern const FT_CALL_STRUCT_PTR FT_FTP;      /* not implemented */

/*
** TFTP data structure
*/
typedef struct tftp_data_struct {
   _ip_address SERVER;
   char_ptr    FILENAME;
   char_ptr    FILEMODE;
} TFTP_DATA_STRUCT, _PTR_ TFTP_DATA_STRUCT_PTR;

/*
** File Format information structure
*/
typedef struct ff_call_struct {
   uint_32     (_CODE_PTR_ START)   (pointer ff_data);
   uint_32     (_CODE_PTR_ DECODE)  (uint_32 size, uchar_ptr data);
   uint_32     (_CODE_PTR_ EOFD)    (void);
   void        (_CODE_PTR_ EXEC)    (void);
} FF_CALL_STRUCT, _PTR_ FF_CALL_STRUCT_PTR;

extern const FF_CALL_STRUCT_PTR FF_BIN;
extern const FF_CALL_STRUCT_PTR FF_BIN32;    /* not implemented */
extern const FF_CALL_STRUCT_PTR FF_SREC;
extern const FF_CALL_STRUCT_PTR FF_SREC32;
extern const FF_CALL_STRUCT_PTR FF_COFF;

/*
** Binary file data structure
*/
typedef struct binfile_data_struct {
   uchar_ptr   DOWNLOAD_ADDR;
   uchar_ptr   EXEC_ADDR;
} BINFILE_DATA_STRUCT, _PTR_ BINFILE_DATA_STRUCT_PTR;


/*
** Initialization for the DHCP server
*/
typedef struct dhcpsrv_data_struct {

   _ip_address SERVERID;
   uint_32     LEASE;
   _ip_address MASK;

   _ip_address SADDR;
   uchar       SNAME[RTCS_SNAME_LEN];
   uchar       FILE[RTCS_BOOTFILE_LEN];

} DHCPSRV_DATA_STRUCT, _PTR_ DHCPSRV_DATA_STRUCT_PTR;


/*
** Type definitions for gethostbyname()
*/

/*
** Host name information structure
*/
#define MAX_HOST_ALIASES     16
#define MAX_HOST_ADDRESSES   16

typedef struct hostent_struct
{
   char_ptr          h_name;
   char_ptr _PTR_    h_aliases;
   int_16            h_addrtype;
   int_16            h_length;
   char_ptr _PTR_    h_addr_list;

} HOSTENT_STRUCT, _PTR_ HOSTENT_STRUCT_PTR;

/* For 4.2BSD compatability */
#define h_addr h_addr_list[0]

/*
** Host entry structure - a ram version of the hosts file
*/
typedef struct host_entry_struct
{
   uint_32        ip_address;
   char_ptr       host_name;
   char_ptr       aliases[MAX_HOST_ALIASES+1];
} HOST_ENTRY_STRUCT;

/*
* extern struct to input host list into
*/
extern const HOST_ENTRY_STRUCT RTCS_Hosts_list[];

typedef struct internal_hostent_struct
{
   HOSTENT_STRUCT   HOSTENT;
   char_ptr         ALIASES[MAX_HOST_ALIASES+1];
   uint_32_ptr      ADDRESSES[MAX_HOST_ADDRESSES+1];
   uint_32          IP_address;
} INTERNAL_HOSTENT_STRUCT, _PTR_ INTERNAL_HOSTENT_STRUCT_PTR;

/*
** Structure to synchronize between applications and
** the TCP/IP task.
*/

typedef struct tcpip_parm {
   pointer        SYNC;       /* for synchronization with application */
   uint_32        ERROR;      /* [OUT] error code */
} TCPIP_PARM, _PTR_ TCPIP_PARM_PTR;

#ifdef __cplusplus
extern "C" {
#endif


/*
** Prototypes for the RTCS_protocol_table[]
*/

extern uint_32 IGMP_init  (void);
extern uint_32 UDP_init   (void);
extern uint_32 TCP_Init   (void);
extern uint_32 RIP_init   (void);
extern uint_32 OSPF_init  (void);
extern uint_32 IPIP_init  (void);


/*
** BSD prototypes
*/

extern uint_32  RTCS_socket
(
   uint_32              ,  /* [IN] protocol family */
   uint_32              ,  /* [IN] type of communication */
   uint_32                 /* [IN] select a specific protocol */
);
extern uint_32  RTCS_shutdown
(
   uint_32              ,  /* [IN] socket handle */
   uint_32                 /* [IN] shutdown method */
);

extern int_32        inet_aton (const char _PTR_, in_addr _PTR_);
extern _ip_address   inet_addr (const char _PTR_);


#if RTCSCFG_ENABLE_DNS

extern HOSTENT_STRUCT _PTR_ DNS_gethostbyname
(
   char _PTR_  name_ptr
);

extern HOSTENT_STRUCT _PTR_ DNS_gethostbyaddr
(
   char _PTR_ addr_ptr,
   uint_32    len,
   uint_32    type
);

extern void DNS_free_HOSTENT_STRUCT
(
   HOSTENT_STRUCT _PTR_ host_ptr
);

extern HOSTENT_STRUCT_PTR RTCS_gethostbyname
(
   char _PTR_    hostname    /* [IN] name of host to search for */
);

#endif


#if RTCSCFG_CHECK_ERRORS

   #define RTCS_CHECKSOCK(s,err) \
                       ((s) == 0) || ((uint_32)(s) == RTCS_SOCKET_ERROR) \
                     ? (err) \
                     :

   #define RTCS_CHECKAPI(f,s,err) \
                       ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f == NULL \
                     ? (err) \
                     :

   #define RTCS_CHECKLEVEL(v,err) \
                       (v) == 0 \
                     ? (err) \
                     :

#else

   #define RTCS_CHECKSOCK(s,err)
   #define RTCS_CHECKAPI(f,s,err)
   #define RTCS_CHECKLEVEL(v,err)

#endif

#if RTCSCFG_CHECK_VALIDITY

   #define RTCS_CHECKVALID(s,err) \
                       ((SOCKET_STRUCT_PTR)(s))->VALID != SOCKET_VALID \
                     ? (err) \
                     :

#else

   #define RTCS_CHECKVALID(s,err)

#endif

#define RTCS_API(f,s,p)             RTCS_CHECKSOCK(s,RTCSERR_SOCK_INVALID) \
                                    RTCS_CHECKVALID(s,RTCSERR_SOCK_INVALID) \
                                    RTCS_CHECKAPI(f,s,RTCSERR_SOCK_NOT_SUPPORTED) \
                                    ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f p

#define RTCS_API_SOCK(f,s,p)        RTCS_CHECKSOCK(s,RTCS_SOCKET_ERROR) \
                                    RTCS_CHECKVALID(s,RTCS_SOCKET_ERROR) \
                                    RTCS_CHECKAPI(f,s,RTCS_SOCKET_ERROR) \
                                    ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f p

#define RTCS_API_SENDRECV(f,s,p)    RTCS_CHECKSOCK(s,RTCS_ERROR) \
                                    RTCS_CHECKVALID(s,RTCS_ERROR) \
                                    RTCS_CHECKAPI(f,s,RTCS_ERROR) \
                                    ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f p

#define RTCS_API_SOCKOPT(f,s,v,p)   RTCS_CHECKLEVEL(v,RTCSERR_SOCK_INVALID_OPTION) \
                                    ((RTCS_SOCKOPT_CALL_STRUCT_PTR)(v))->SOCK_ ## f p

#define bind(s,ap,al)            (RTCS_API(BIND,        s, (s,(const sockaddr *)(ap),al)))
#define connect(s,ap,al)         (RTCS_API(CONNECT,     s, (s,(sockaddr *)(ap),al)))
#define listen(s,f)              (RTCS_API(LISTEN,      s, (s,f)))
#define getsockname(s,ap,al)     (RTCS_API(GETSOCKNAME, s, (s,(sockaddr *)(ap),al)))
#define getpeername(s,ap,al)     (RTCS_API(GETPEERNAME, s, (s,(sockaddr *)(ap),al)))
#define accept(s,ap,al)          (RTCS_API_SOCK(ACCEPT, s, (s,(sockaddr *)(ap),al)))
#define send(s,p,l,f)            (RTCS_API_SENDRECV(SEND,      s, (s,p,l,f)))
#define sendto(s,p,l,f,ap,al)    (RTCS_API_SENDRECV(SENDTO,    s, (s,p,l,f,(sockaddr *)(ap),al)))
#define recv(s,p,l,f)            (RTCS_API_SENDRECV(RECV,      s, (s,p,l,f)))
#define recvfrom(s,p,l,f,ap,al)  (RTCS_API_SENDRECV(RECVFROM,  s, (s,p,l,f,(sockaddr *)(ap),al)))
#define getsockopt(s,v,on,op,ol) (RTCS_API_SOCKOPT(GETSOCKOPT, s, v, (s,v,on,op,ol)))
#define setsockopt(s,v,on,op,ol) (RTCS_API_SOCKOPT(SETSOCKOPT, s, v, (s,v,on,op,ol)))

#define socket          RTCS_socket
#define shutdown        RTCS_shutdown


#if RTCSCFG_ENABLE_DNS

#define gethostbyname   DNS_gethostbyname
#define gethostbyaddr   DNS_gethostbyaddr

#endif

/*
** RTCS (non-BSD) prototypes
*/

extern uint_32  RTCS_attachsock
(
   uint_32                       /* [IN] socket handle */
);
extern uint_32  RTCS_detachsock
(
   uint_32                       /* [IN] socket handle */
);
extern uint_32  RTCS_transfersock
(
   uint_32     in_sock,          /*[IN] specifies the handle of the existing socket */
   _task_id    new_owner
);
extern uint_32  RTCS_selectall
(
   uint_32                       /* [IN] time to wait for data, in milliseconds */
);

#define RTCS_selectset(sockset, size, timeout)  _RTCS_selectset(sockset, size, timeout)

extern uint_32  _RTCS_select
(
   pointer                    ,  /* [IN] set of read sockets to block on */
   uint_32                    ,  /* [IN] size of read socket set */
   pointer                    ,  /* [IN] set of write sockets to block on */
   uint_32                    ,  /* [IN] size of write socket set */
   uint_32                       /* [IN] time to wait for data, in milliseconds */
);

extern uint_32 _RTCS_selectset
(
   pointer sockset,
   uint_32 size,
   uint_32 timeout
);

extern uint_32  RTCS_geterror
(
   uint_32                       /* [IN] socket handle */
);

extern uint_32 RTCS_load
(
   FT_CALL_STRUCT_PTR         ,  /* [IN] file transfer protocol to use */
   pointer                    ,  /* [IN] file transfer data */
   FF_CALL_STRUCT_PTR         ,  /* [IN] file format to decode */
   pointer                    ,  /* [IN] file format data */
   uint_32                       /* [IN] optional flags */
);

extern uint_32 RTCS_load_TFTP_BIN
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                   ,  /* [IN] file to retrieve */
   uchar_ptr                     /* [IN] download start address */
);

extern uint_32 RTCS_exec_TFTP_BIN
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                   ,  /* [IN] file to retrieve */
   uchar_ptr                  ,  /* [IN] download start address */
   uchar_ptr                     /* [IN] execution start address */
);

extern uint_32 RTCS_load_TFTP_SREC
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                      /* [IN] file to retrieve */
);

extern uint_32 RTCS_exec_TFTP_SREC
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                      /* [IN] file to retrieve */
);

extern uint_32 RTCS_load_TFTP_COFF
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                      /* [IN] file to retrieve */
);

extern uint_32 RTCS_exec_TFTP_COFF
(
   _ip_address                ,  /* [IN] address of the TFTP server */
   char_ptr                      /* [IN] file to retrieve */
);

#if RTCSCFG_ENABLE_IP_STATS
extern IP_STATS_PTR   IP_stats   (void);
#endif
#if RTCSCFG_ENABLE_ICMP_STATS
extern ICMP_STATS_PTR ICMP_stats (void);
#endif
#if RTCSCFG_ENABLE_UDP_STATS
extern UDP_STATS_PTR  UDP_stats  (void);
#endif
#if RTCSCFG_ENABLE_TCP_STATS
extern TCP_STATS_PTR  TCP_stats  (void);
#endif
#if RTCSCFG_ENABLE_IPIF_STATS
extern IPIF_STATS_PTR IPIF_stats (_rtcs_if_handle);
#endif
#if RTCSCFG_ENABLE_ARP_STATS
extern ARP_STATS_PTR  ARP_stats  (_rtcs_if_handle);
#endif
#if RTCSCFG_ENABLE_IP6_STATS
extern IP6_STATS_PTR   IP6_stats (void);
#endif
#if RTCSCFG_ENABLE_ICMP6_STATS
extern ICMP6_STATS_PTR ICMP6_stats  (void);
#endif

extern uint_32 ECHOSRV_init       (char_ptr, uint_32, uint_32);
extern uint_32 TELNETSRV_run     ( RTCS_TASK_PTR );

extern uint_32 TELNETSRV_init     (char_ptr, uint_32, uint_32, RTCS_TASK_PTR);
extern uint_32 TELNETSRV_sockinit (char_ptr, uint_32, uint_32, RTCS_TASK_PTR, uint_32);
extern uint_32 DHCPSRV_init       (char_ptr, uint_32, uint_32);
extern uint_32 TFTPSRV_init       (char_ptr, uint_32, uint_32);
extern uint_32 SNTP_init          (char_ptr, uint_32, uint_32, _ip_address, uint_32);
extern uint_32 SNMP_init          (char_ptr, uint_32, uint_32);

extern uint_32 SNMP_init_with_traps (char_ptr, uint_32, uint_32, _ip_address _PTR_);

extern uint_32 EDS_init           (char_ptr, uint_32, uint_32);

extern uint_32 EDS_stop           (void);
extern uint_32 ECHOSRV_stop       (void);
extern uint_32 SNMP_stop          (void);
extern uint_32 TFTPSRV_stop       (void);
extern uint_32 TELNETSRV_stop     (void);

extern boolean TFTPSRV_access (char_ptr, uint_16);

extern void    RTCS_delay (uint_32);
extern void    RTCS_shell (void);
extern uint_32 RTCS_ping  (_ip_address, uint_32_ptr timeout, uint_16 id, uint_8  hop_limit, pointer data_buffer, uint_32 data_buffer_size);


extern uint_32 TELNET_connect   (_ip_address);

extern uint_32 SNTP_oneshot     (_ip_address, uint_32);

extern void MIB1213_init (void);
extern void MIBMQX_init  (void);

/* Obsolete -- provided for compatility */
extern void RTCS_netstat_task (uint_32);
extern void ECHO_task         (uint_32);
extern void TELNET_server     (uint_32);
extern void EDS_task          (uint_32);
#define TELNET_client RTCS_netstat_task

/*
** RTCS initialization prototypes
*/

extern uint_32 RTCS_create
(
   void
);

extern uint_32 RTCS_if_add
(
   pointer                    ,  /* [IN] the packet driver handle */
   RTCS_IF_STRUCT_PTR         ,  /* [IN] call table for the interface */
   _rtcs_if_handle _PTR_         /* [OUT] the RTCS interface state structure */
);
extern uint_32 RTCS_if_remove
(
   _rtcs_if_handle               /* [IN] the RTCS interface state structure */
);
extern uint_32 RTCS_if_bind
(
   _rtcs_if_handle   handle   ,  /* [IN] the RTCS interface state structure */
   _ip_address       address  ,  /* [IN] the IP address for the interface */
   _ip_address       netmask     /* [IN] the IP (sub)network mask for the interface */
);
extern uint_32 RTCS_if_probe_and_bind
(
   _rtcs_if_handle   handle   ,  /* [IN] the RTCS interface state structure */
   _ip_address       address  ,  /* [IN] the IP address for the interface */
   _ip_address       netmask     /* [IN] the IP (sub)network mask for the interface */
);
extern uint_32 RTCS_if_bind_BOOTP
(
   _rtcs_if_handle            ,
   BOOTP_DATA_STRUCT_PTR
);
extern uint_32 RTCS_if_bind_IPCP
(
   _rtcs_if_handle            ,
   IPCP_DATA_STRUCT_PTR
);

extern uint_32 RTCS_arp_add
(
   _rtcs_if_handle _PTR_   ihandle,  /* [IN] the RTCS interface state structure */
   _ip_address              paddr,   /* [IN] the address to add */
   char           laddr[6]
);
extern uint_32 RTCS_arp_delete
(
   _rtcs_if_handle _PTR_   ihandle,  /* [IN] the RTCS interface state structure */
   _ip_address                paddr  /* [IN] the address to add */
);

extern uint_32 RTCS_if_unbind
(
   _rtcs_if_handle   handle   ,  /* [IN] the RTCS interface state structure */
   _ip_address       address     /* [IN] the IP address for the interface */
);
extern uint_32 RTCS_gate_add
(
   _ip_address       gateway  ,  /* [IN] the IP address of the gateway */
   _ip_address       network  ,  /* [IN] the IP (sub)network to route */
   _ip_address       netmask     /* [IN] the IP (sub)network mask to route */
);

extern uint_32 RTCS_gate_add_metric
(
   _ip_address       gateway  ,  /* [IN] the IP address of the gateway */
   _ip_address       network  ,  /* [IN] the IP (sub)network to route */
   _ip_address       netmask  ,  /* [IN] the IP (sub)network mask to route */
   uint_16           metric      /* [IN] the metric of the gateway */
);

extern uint_32 RTCS_gate_remove
(
   _ip_address       gateway  ,  /* [IN] the IP address of the gateway */
   _ip_address       network  ,  /* [IN] the IP (sub)network to route */
   _ip_address       netmask     /* [IN] the IP (sub)network mask to route */
);

extern uint_32 RTCS_gate_remove_metric
(
   _ip_address       gateway  ,  /* [IN] the IP address of the gateway */
   _ip_address       network  ,  /* [IN] the IP (sub)network to route */
   _ip_address       netmask  ,  /* [IN] the IP (sub)network mask to route */
   uint_16           metric      /* [IN] the metric of the gateway */
);

extern _ip_address RTCS_get_route
(
   _ip_address netaddr, /* [IN] the network address */
   _ip_address netmask  /* [IN] the network mask */
);
extern void RTCS_walk_route( void );

extern uint_32 RTCS_trap_target_add
(
   _ip_address       ip_addr     /* [IN] the IP address to send the trap to */
);

extern uint_32 RTCS_trap_target_remove
(
   _ip_address       ip_addr     /* [IN] removes this IP address from the trap list */
);


extern _ip_address  IPCP_get_local_addr( _rtcs_if_handle );
extern _ip_address  IPCP_get_peer_addr( _rtcs_if_handle );


#if RTCSCFG_ENABLE_DNS

extern uint_32 DNS_init
(
   void
);

#endif

#if RTCSCFG_ENABLE_LWDNS

boolean LWDNS_resolve(
   char_ptr          name_to_resolve,
   _ip_address _PTR_ ipaddr_ptr,
   char_ptr          ipname,
   uint_32           ipnamesize
   );

extern _ip_address LWDNS_server_ipaddr;

#endif

extern boolean RTCS_resolve_ip_address( char_ptr arg, _ip_address _PTR_ ipaddr_ptr, char_ptr ipname, uint_32 ipnamesize);


extern void  RTCS_log_error
(
   uint_32  module,
   uint_32  error,
   uint_32  p1,
   uint_32  p2,
   uint_32  p3
);


#include <dhcp.h>

/*
** The DHCP server
*/

#define DHCPSRV_option_int8     DHCP_option_int8
#define DHCPSRV_option_int16    DHCP_option_int16
#define DHCPSRV_option_int32    DHCP_option_int32
#define DHCPSRV_option_addr     DHCP_option_addr
#define DHCPSRV_option_addrlist DHCP_option_addrlist
#define DHCPSRV_option_string   DHCP_option_string

extern uint_32 DHCPSRV_ippool_add
(
   _ip_address             ipstart,
   uint_32                 ipnum,
   DHCPSRV_DATA_STRUCT_PTR params,
   uchar_ptr               optptr,
   uint_32                 optlen
);

/*
** The IPIP device
*/

extern uint_32 RTCS_tunnel_add
(
   _ip_address  inner_source_addr,     /* [IN] Inner IP header source        */
   _ip_address  inner_source_netmask,  /* [IN] Inner IP header source mask   */
   _ip_address  inner_dest_addr,       /* [IN] Inner IP header dest          */
   _ip_address  inner_dest_netmask,    /* [IN] Inner IP header dest mask     */
   _ip_address  outer_source_addr,     /* [IN] Outer IP header source        */
   _ip_address  outer_dest_addr,       /* [IN] Outer IP header dest          */
   uint_32 flags                       /* [IN] Flags to set tunnel behaviour */
);

extern uint_32 RTCS_tunnel_remove
(
   _ip_address  inner_source_addr,     /* [IN] Inner IP header source        */
   _ip_address  inner_source_netmask,  /* [IN] Inner IP header source mask   */
   _ip_address  inner_dest_addr,       /* [IN] Inner IP header dest          */
   _ip_address  inner_dest_netmask,    /* [IN] Inner IP header dest mask     */
   _ip_address  outer_source_addr,     /* [IN] Outer IP header source        */
   _ip_address  outer_dest_addr,       /* [IN] Outer IP header dest          */
   uint_32 flags                       /* [IN] Flags to set tunnel behaviour */
);

/*
** Util function
*/

extern pointer RTCS_get_enet_handle
(
   _rtcs_if_handle   ihandle           /* [IN] Interface */
);


extern uint_32 RTCS6_if_bind_addr (_rtcs_if_handle handle, in6_addr ip_addr, ip6_if_addr_type ip_addr_type);
extern uint_32 RTCS6_if_unbind_addr (_rtcs_if_handle  handle, in6_addr ip_addr);
extern uint_32 RTCS6_ping(sockaddr *ip_addr, uint_32_ptr timeout, uint_16 id, uint_8  hop_limit, pointer ping_data_buffer, uint_32 ping_data_len);

#ifdef __cplusplus
}
#endif

/*
** IPv6 address definition (BSD-like)
*/
//  TBD:
//  To avoid optimisation problem, size of sockaddr_in6 can be different
//  so, we can try to declare sa_data[22] like:
//  sa_data[sifeoff(sockaddr_in6) - sizeof(sockaddr_in6.sin6_family)]
//  typedef struct sockaddr {
//  	uint_16		sa_family;
//  	char 		sa_data[22];
//  } sockaddr;

typedef struct addrinfo {
	uint_16		    ai_flags			;	/* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
	uint_16  	    ai_family			;	/* AF_INET, AF_INET6						*/
	uint_32		    ai_socktype			;	/* SOCK_STREAM, SOCK_DGRAM					*/
	uint_16		    ai_protocol			;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 		*/
	unsigned int 	ai_addrlen			;	/* length of ai_addr 						*/
	char	        *ai_canonname		;	/* canonical name for hostname 				*/
	struct		    sockaddr *ai_addr	;	/* binary address 							*/
	struct		    addrinfo *ai_next	;	/* next structure in linked list 			*/
} addrinfo;

/* user api functions */

extern int_32 inet_pton
(
    int_32 		  af, 	    /* af - address family (AF_INET or AF_INET6)				    */
    const char 	  *src, 	    /* textual presentation of an address,example:"192.0.2.12" 	    */
    void 		  *dst,       /* pointer to a in_addr or in6-addr binary form of an address   */
    unsigned int  sizeof_dst  /* size of dst buffer                                           */
);

extern char *inet_ntop
(
	int_32 			af, 		/* af - address family (AF_INET or AF_INET6)				*/
	const void 		*src, 		/* pointer to a binary form of an address					*/
	char 			*dst, 		/* pointer to a textual presentation of an address			*/
	unsigned int	size		/* max. size of the textual presentation of an address		*/
);

extern unsigned int strlcpy
(
	char 				   *dst, 		/* pointer to destination buffer 					*/
	const char 			   *src, 		/* pointer to source string		 					*/
	unsigned int	       siz			/* size of the destination buffer					*/
);

extern void freeaddrinfo
(
	struct addrinfo 	   *ai			/* addr info structure 								*/
);

extern int_32 getnameinfo
(
	const struct sockaddr   *sa		,	/* socket address structure 						*/
	unsigned int            salen	, 	/* size of socket address structure 				*/
	char 				    *host	,	/* pointer to host name 							*/
	unsigned int            hostlen	, 	/* size of host 									*/
	char       			    *serv	, 	/* pointer to service 								*/
	unsigned int            servlen	, 	/* size of serv 									*/
	int                     flags		/* flags 											*/
	/*
        NI_NUMERICHOST Return the address in numeric form.
	*/
);

extern int_32 getaddrinfo
(
	const char 				*hostname, 	/* host name or IP or NULL							*/
	const char 				*servname,	/* service name or port number						*/
	const struct addrinfo 	*hints,		/* set flags										*/
	struct addrinfo 		**res		/* [OUT]list of address structures					*/
);

#endif


/* EOF */
