/**HEADER********************************************************************
* 
* Copyright (c) 2008-2011 Freescale Semiconductor;
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
* $FileName: rtcscfg.h$
* $Version : 3.8.54.0$
* $Date    : Oct-9-2012$
*
* Comments:
*
*   This file contains the definitions for configuring
*   optional features in RTCS.
*   RTCS is delivered as follows:
*   RTCSCFG_CHECK_ERRORS                   1
*   RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS 1
*   RTCSCFG_CHECK_VALIDITY                 1
*   RTCSCFG_CHECK_ADDRSIZE                 1
*   RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST  0
*   RTCSCFG_BOOTP_RETURN_YIADDR            0
*   RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST    0  
*   RTCSCFG_LOG_SOCKET_API                 1
*   RTCSCFG_LOG_PCB                        1
*   RTCSCFG_LINKOPT_8023                   0
*   RTCSCFG_LINKOPT_8021Q_PRIO             0
*   RTCSCFG_DISCARD_SELF_BCASTS            1
*
*   RTCSCFG_LOGGING                        0
*
*END************************************************************************/

#ifndef __rtcscfg_h__
#define __rtcscfg_h__

/***************************************************************************
**
** Code and Data configuration options
** MGCT: <category name="Protocol and Features Options">
**
*/

/* 
** IPv6 Protocol
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP6
   #define RTCSCFG_ENABLE_IP6		0
#endif

/* 
** IPv4 Protocol
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP4
   #define RTCSCFG_ENABLE_IP4		1
#endif


/*
** Global setting to reduce RTCS memory requirements. This affects lot of
** other settings.
**
** MGCT: <option type="bool"/>
*/
#ifndef RTCS_MINIMUM_FOOTPRINT
    #define RTCS_MINIMUM_FOOTPRINT 0
#endif

/* 
** ICMPv4 Protocol
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_ICMP
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_ICMP 0
    #else
        #define RTCSCFG_ENABLE_ICMP 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IGMP
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_IGMP 0
    #else
        #define RTCSCFG_ENABLE_IGMP 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_NAT
   #define RTCSCFG_ENABLE_NAT  0 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_DNS
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_DNS 0
    #else
        #define RTCSCFG_ENABLE_DNS 1
    #endif
#endif

#if RTCSCFG_ENABLE_DNS == 1
    #ifdef RTCSCFG_ENABLE_LOOPBACK
        #undef  RTCSCFG_ENABLE_LOOPBACK      
    #endif
    #define RTCSCFG_ENABLE_LOOPBACK 1
    #ifdef RTCSCFG_ENABLE_LWDNS
        #undef  RTCSCFG_ENABLE_LWDNS
    #endif
    #define RTCSCFG_ENABLE_LWDNS 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_LWDNS
   #define RTCSCFG_ENABLE_LWDNS  1 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IPSEC
   #define RTCSCFG_ENABLE_IPSEC 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IPIP
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_IPIP 0
    #else
        #define RTCSCFG_ENABLE_IPIP 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_RIP
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_RIP  0
    #else
        #define RTCSCFG_ENABLE_RIP  1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_OSPF
   #define RTCSCFG_ENABLE_OSPF 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_SNMP
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_SNMP 0
    #else
        #define RTCSCFG_ENABLE_SNMP 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP_REASSEMBLY
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_IP_REASSEMBLY 0
    #else
        #define RTCSCFG_ENABLE_IP_REASSEMBLY 1
    #endif 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_LOOPBACK
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_LOOPBACK 0
    #else
        #define RTCSCFG_ENABLE_LOOPBACK 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_UDP
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_UDP 0
    #else
        #define RTCSCFG_ENABLE_UDP 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_TCP
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_TCP 0
    #else
        #define RTCSCFG_ENABLE_TCP 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_STATS
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_STATS 0
    #else
        #define RTCSCFG_ENABLE_STATS 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_GATEWAYS
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_GATEWAYS 0
    #else
        #define RTCSCFG_ENABLE_GATEWAYS 1
    #endif
#endif

/*
** Note: must be true for PPP or tunneling
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_VIRTUAL_ROUTES
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_ENABLE_VIRTUAL_ROUTES 0
    #else
        #define RTCSCFG_ENABLE_VIRTUAL_ROUTES 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_USE_KISS_RNG 
   #define RTCSCFG_USE_KISS_RNG 0
#endif

/* MGCT: </category> */

/***************************************************************************
**
** Error Checking options
** 
** MGCT: <category name="Error Checking Options">
**
****************************************************************************/

/*
** When RTCSCFG_CHECK_ERRORS is 1, RTCS API functions will perform
** error checking on its input parameters and also some other.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_CHECK_ERRORS
   #define RTCSCFG_CHECK_ERRORS 1
#endif

/*
** When RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS is 1, RTCS API functions
** will perform error checking on memory allocation.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS
   #define RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS 1
#endif

/*
** When RTCSCFG_CHECK_VALIDITY is 1, RTCS will check the VALID field
** in internal structures before accessing them.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_CHECK_VALIDITY
   #define RTCSCFG_CHECK_VALIDITY 1
#endif

/*
** When RTCSCFG_CHECK_ADDRSIZE is 1, RTCS will check whether addrlen
** is at least sizeof(sockaddr_in) in functions that take a struct
** sockaddr _PTR_ parameter.  If not, RTCS returns an error (for bind(),
** connect() and sendto()) or does a partial copy (for accept(),
** getsockname(), getpeername() and recvfrom()).
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_CHECK_ADDRSIZE
   #define RTCSCFG_CHECK_ADDRSIZE 1
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** PCB Configuration options
**
** MGCT: <category name="PCB Configuration Options">
**
*/

/*
** Override in application by setting _RTCSPCB_max
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_PCBS_INIT
   #if RTCS_MINIMUM_FOOTPRINT
      #define RTCSCFG_PCBS_INIT 4
   #else
      #define RTCSCFG_PCBS_INIT 32
   #endif
#endif

/*
** Override in application by setting _RTCSPCB_max
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_PCBS_GROW
   #define RTCSCFG_PCBS_GROW 0 
#endif

/*
** Override in application by setting _RTCSPCB_max
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_PCBS_MAX
   #define RTCSCFG_PCBS_MAX RTCSCFG_PCBS_INIT 
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** Socket Configuration options
**
** MGCT: <category name="Socket Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_SOCKET_OWNERSHIP
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_SOCKET_OWNERSHIP 0
    #else
        #define RTCSCFG_SOCKET_OWNERSHIP 1
    #endif
#endif

/*
** Number of blocks allocated for sockets
** Override in application by setting _RTCS_socket_part_init
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_SOCKET_PART_INIT
   #if RTCS_MINIMUM_FOOTPRINT
      #define RTCSCFG_SOCKET_PART_INIT 3 
   #else
      #define RTCSCFG_SOCKET_PART_INIT 20
   #endif
#endif

/*
** Override in application by setting _RTCS_socket_part_grow
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_SOCKET_PART_GROW
   #if RTCS_MINIMUM_FOOTPRINT
      #define RTCSCFG_SOCKET_PART_GROW 0 
   #else
      #define RTCSCFG_SOCKET_PART_GROW 20
   #endif
#endif

/*
** Override in application by setting _RTCS_socket_part_limit
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_SOCKET_PART_MAX
   #define RTCSCFG_SOCKET_PART_MAX RTCSCFG_SOCKET_PART_INIT 
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** RTCS Message Configuration options
**
** MGCT: <category name="RTCS Task and Message Options">
**
*/

/*
** Override in application by setting _RTCS_msgpool_init
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_MSGPOOL_INIT
   #if RTCS_MINIMUM_FOOTPRINT
      #define RTCSCFG_MSGPOOL_INIT 8 
   #else
      #define RTCSCFG_MSGPOOL_INIT 32
   #endif
#endif

/*
** Override in application by setting _RTCS_msgpool_grow
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_MSGPOOL_GROW
   #if RTCS_MINIMUM_FOOTPRINT
      #define RTCSCFG_MSGPOOL_GROW 2 
   #else
      #define RTCSCFG_MSGPOOL_GROW 16
   #endif
#endif

/*
** Override in application by setting _RTCS_msgpool_limit
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_MSGPOOL_MAX
   #if RTCS_MINIMUM_FOOTPRINT
      #define RTCSCFG_MSGPOOL_MAX RTCSCFG_MSGPOOL_INIT 
   #else
      #define RTCSCFG_MSGPOOL_MAX 160
   #endif
#endif

/* MGCT: </category> */

/***************************************************************************
**
** Link Layer options
**
** MGCT: <category name="Link Layer Options">
**
*/

/*
** RTCSCFG_LINKOPT_8023 enables support for sending and receiving
** 802.3 frames.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LINKOPT_8023
   #define RTCSCFG_LINKOPT_8023 0
#endif

/*
** RTCSCFG_LINKOPT_8021Q_PRIO enables support for sending and receiving
** 802.1Q priority tags.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LINKOPT_8021Q_PRIO
   #define RTCSCFG_LINKOPT_8021Q_PRIO 0
#endif

/* 
** RTCSCFG_DISCARD_SELF_BCASTS controls whether or not to discard all 
** broadcast pkts that we sent, as they are likely echoes from older 
** hubs
** MGCT: <option type="bool"/>
*/ 
#ifndef RTCSCFG_DISCARD_SELF_BCASTS
   #define RTCSCFG_DISCARD_SELF_BCASTS 1
#endif

/* MGCT: </category> */


/***************************************************************************
** 
** ARP Configuration options
**
** MGCT: <category name="ARP Configuration Options">
**
*/

/*
** ARP cache hash table size
** MGCT: <option type="number"/>
*/
#ifndef ARPCACHE_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define ARPCACHE_SIZE 2
   #else
      #define ARPCACHE_SIZE 16
   #endif
#endif

/*
** Allocation blocks for ARP cache entries
** MGCT: <option type="number"/>
*/
#ifndef ARPALLOC_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define ARPALLOC_SIZE 2 
   #else
      #define ARPALLOC_SIZE 16
   #endif
#endif

/*
** ARP cache entry longevity (in milliseconds)
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ARPTIME_RESEND_MIN
    #define RTCSCFG_ARPTIME_RESEND_MIN             5000     /*  5 sec */
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ARPTIME_RESEND_MAX  
    #define RTCSCFG_ARPTIME_RESEND_MAX            30000     /* 30 sec */
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ARPTIME_EXPIRE_INCOMPLETE
    #define RTCSCFG_ARPTIME_EXPIRE_INCOMPLETE    180000L    /*  3 min */
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ARPTIME_EXPIRE_COMPLETE
    #define RTCSCFG_ARPTIME_EXPIRE_COMPLETE     1200000L    /* 20 min */
#endif

/*
** Number of PCBs queued on an outstanding ARP
** Per RFC, Minimum recommended is 1.
** When sending large UDP packets, which will result 
** in IP fragmentations, set to at least the largest number of 
** fragments
** MGCT: <option type="number"/>
*/
#ifndef ARP_ENTRY_MAX_QUEUED_PCBS
    #define ARP_ENTRY_MAX_QUEUED_PCBS 1 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_ARP_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_ARP_STATS 1
    #else
        #define RTCSCFG_ENABLE_ARP_STATS 0
    #endif
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** IPv4 Configuration options
**
** MGCT: <category name="IPv4 Configuration Options">
**
*/

/*
** MGCT: <option type="number"/>
*/
#ifndef IPIFALLOC_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define IPIFALLOC_SIZE 2 
   #else
      #define IPIFALLOC_SIZE 4
   #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef IPROUTEALLOC_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define IPROUTEALLOC_SIZE 2 
   #else
      #define IPROUTEALLOC_SIZE 4
   #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef IPGATEALLOC_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define IPGATEALLOC_SIZE 1 
   #else
      #define IPGATEALLOC_SIZE 4
   #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef IPMCBALLOC_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define IPMCBALLOC_SIZE 1 
   #else
      #define IPMCBALLOC_SIZE 4
   #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef RADIXALLOC_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define RADIXALLOC_SIZE 2 
   #else
      #define RADIXALLOC_SIZE 4
   #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_IP_STATS 1
    #else
        #define RTCSCFG_ENABLE_IP_STATS 0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_ICMP_STATS
    #if (RTCSCFG_ENABLE_STATS & RTCSCFG_ENABLE_ICMP)
        #define RTCSCFG_ENABLE_ICMP_STATS 1
    #else
        #define RTCSCFG_ENABLE_ICMP_STATS 0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IPIF_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_IPIF_STATS 1
    #else
        #define RTCSCFG_ENABLE_IPIF_STATS 0
    #endif
#endif

#ifndef RTCSCFG_ENABLE_ICMP6_STATS
   #define RTCSCFG_ENABLE_ICMP6_STATS RTCSCFG_ENABLE_STATS
#endif

/* MGCT: </category> */


/***************************************************************************
** 
** IGMP Configuration options
**
** MGCT: <category name="IGMP Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IGMP_STATS
    #if (RTCSCFG_ENABLE_STATS & RTCSCFG_ENABLE_IGMP)
        #define RTCSCFG_ENABLE_IGMP_STATS 1
    #else
        #define RTCSCFG_ENABLE_IGMP_STATS 0
    #endif
#endif

#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
   #ifdef BSP_ENET_DEVICE_COUNT
   #if  (BSP_ENET_DEVICE_COUNT > 0) 
      #if !(BSPCFG_ENABLE_ENET_MULTICAST)
         #error RTCS IGMP uses ENET Multicast.  Enable BSPCFG_ENABLE_ENET_MULTICAST or disable RTCSCFG_ENABLE_IGMP in user_config.h
      #endif
   #endif
   #endif
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** UDP Configuration options
**
** MGCT: <category name="UDP Configuration Options">
**
*/

/*
** Override in application by setting _UDP_max_queue_size
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_UDP_MAX_QUEUE_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define RTCSCFG_UDP_MAX_QUEUE_SIZE 1 
   #else
      #define RTCSCFG_UDP_MAX_QUEUE_SIZE (RTCSCFG_PCBS_MAX/2)
   #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_UDP_STATS
    #if (RTCSCFG_ENABLE_STATS & RTCSCFG_ENABLE_UDP)
        #define RTCSCFG_ENABLE_UDP_STATS 1
    #else
        #define RTCSCFG_ENABLE_UDP_STATS 0
    #endif
#endif


/* MGCT: </category> */

/***************************************************************************
** 
** TCP Configuration options
**
** MGCT: <category name="TCP Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_TCP_STATS
    #if (RTCSCFG_ENABLE_STATS & RTCSCFG_ENABLE_TCP)
        #define RTCSCFG_ENABLE_TCP_STATS 1
    #else
        #define RTCSCFG_ENABLE_TCP_STATS 0
    #endif
#endif

/*
** Maximum number of simultaneous connections allowed.  Define as 0 for no limit.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_TCP_MAX_CONNECTIONS
   #define RTCSCFG_TCP_MAX_CONNECTIONS      0  
#endif

/*
** Maximum number of simultaneoushalf open connections allowed.
** Define as 0 to disable the SYN attack recovery feature. 
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_TCP_MAX_HALF_OPEN
   #define RTCSCFG_TCP_MAX_HALF_OPEN        0  
#endif

#ifndef RTCSCFG_TCP_ACKDELAY
    #define TCP_ACKDELAY                    1 
#else
    #if RTCSCFG_TCP_ACKDELAY > 500   /* Max ack delay, as per RFC1122 */
        #define TCP_ACKDELAY                500
    #else
        #define TCP_ACKDELAY                RTCSCFG_TCP_ACKDELAY
    #endif
#endif

/* MGCT: </category> */


/***************************************************************************
** 
** RIP Configuration options
**
** MGCT: <category name="RIP Configuration options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_RIP_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_RIP_STATS 1
    #else
        #define RTCSCFG_ENABLE_RIP_STATS 0
    #endif
#endif

#if RTCSCFG_ENABLE_RIP
   #if !(RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4) 
      #error RIP uses IGMP.  Enable RTCSCFG_ENABLE_IGMP or disable RTCSCFG_ENABLE_RIP in user_config.h
   #endif
#endif

/* MGCT: </category> */

/***************************************************************************
**
** Protocol behaviour options
**
** MGCT: <category name="Protocol Behaviour Options">
**
*/

/*
** RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST disables the reception and
** forwarding of directed broadcast datagrams.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST
   #define RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST 1
#endif

/*
** When RTCSCFG_BOOTP_RETURN_YIADDR is 1, the BOOTP_DATA_STRUCT has
** an additional field, which will be filled in with the YIADDR field
** of the BOOTREPLY.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_BOOTP_RETURN_YIADDR
   #define RTCSCFG_BOOTP_RETURN_YIADDR 0
#endif

/*
** When RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST is 1, locally bound sockets
** that are members of multicast groups will be able to receive messages
** sent to both their unicast and multicast addresses.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST
   #define RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST 0
#endif

/* MGCT: </category> */


/***************************************************************************
** 
** RTCS Configuration options
**
** MGCT: <category name="RTCS Configuration Options">
**
*/

/*
** Using a lower queue base requires fewer MQX resources
** Override in application by setting _RTCSQUEUE_base
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_QUEUE_BASE
   #if RTCS_MINIMUM_FOOTPRINT
      #define RTCSCFG_QUEUE_BASE 2 
   #else
      #define RTCSCFG_QUEUE_BASE 0x10
   #endif
#endif

/*
** Override in application by setting _RTCSTASK_stacksize
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_STACK_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define RTCSCFG_STACK_SIZE (750*sizeof(uint_32)) 
   #else
      #define RTCSCFG_STACK_SIZE (1000*sizeof(uint_32))
   #endif
#endif

/* MGCT: </category> */

/***************************************************************************
**
** Logging options
**
** MGCT: <category name="Logging Options">
**
*/

/*
** Enable RTCS logging functionality
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LOGGING
    #define RTCSCFG_LOGGING 0
#endif

/*
** When RTCSCFG_LOG_SOCKET_API is 1, RTCS will call RTCS_log() on
** every socket API entry and exit.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LOG_SOCKET_API
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_LOG_SOCKET_API 0
    #else
        #define RTCSCFG_LOG_SOCKET_API 1
    #endif
#endif

/*
** When RTCSCFG_LOG_PCB is 1, RTCS will call RTCS_log() every time
** a PCB is allocated, freed, or passed between two layers.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LOG_PCB
    #if RTCS_MINIMUM_FOOTPRINT
        #define RTCSCFG_LOG_PCB 0
    #else
        #define RTCSCFG_LOG_PCB 1
    #endif
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** IPCFG Configuration options
**
** MGCT: <category name="IPCFG API Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IPCFG_ENABLE_DNS
    #if (RTCSCFG_ENABLE_DNS & RTCSCFG_ENABLE_UDP) | (RTCSCFG_ENABLE_LWDNS)
        #define RTCSCFG_IPCFG_ENABLE_DNS 1
    #else
        #define RTCSCFG_IPCFG_ENABLE_DNS 0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IPCFG_ENABLE_DHCP
    #if RTCSCFG_ENABLE_UDP
        #define RTCSCFG_IPCFG_ENABLE_DHCP 1
    #else
        #define RTCSCFG_IPCFG_ENABLE_DHCP 0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IPCFG_ENABLE_BOOT
    #define RTCSCFG_IPCFG_ENABLE_BOOT 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_USE_MQX_PARTITIONS
    #define RTCSCFG_USE_MQX_PARTITIONS 1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_USE_INTERNAL_PARTITIONS
    #if RTCSCFG_USE_MQX_PARTITIONS
        #define RTCSCFG_USE_INTERNAL_PARTITIONS 0
    #else
        #define RTCSCFG_USE_INTERNAL_PARTITIONS 1
    #endif
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** FTP Client Configuration options
**
** MGCT: <category name="FTP Client Configuration Options">
**
*/

/*
** Enabled means better performance for files less than 4MB
** MGCT: <option type="bool"/>
*/
#ifndef FTPCCFG_SMALL_FILE_PERFORMANCE_ENHANCEMENT
   #define FTPCCFG_SMALL_FILE_PERFORMANCE_ENHANCEMENT 1 
#endif

/*
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#define FTPCCFG_BUFFER_SIZE 256

/*
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef FTPCCFG_WINDOW_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define FTPCCFG_WINDOW_SIZE 536 
   #else
      #define FTPCCFG_WINDOW_SIZE (3*1460)  
   #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef FTPCCFG_TIMEWAIT_TIMEOUT
   #define FTPCCFG_TIMEWAIT_TIMEOUT 1000
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** FTP Server Configuration options
**
** MGCT: <category name="FTP Server Configuration Options">
**
*/

/*
** must be one of FLAG_ABORT_CONNECTION or FLAG_CLOSE_TX
** MGCT: <option type="list">
** <item name="FLAG_ABORT_CONNECTION" value="FLAG_ABORT_CONNECTION"/>
** <item name="FLAG_CLOSE_TX" value="FLAG_CLOSE_TX"/>
** </option>
*/
#ifndef FTPDCFG_SHUTDOWN_OPTION
   #define FTPDCFG_SHUTDOWN_OPTION FLAG_ABORT_CONNECTION  
#endif

/*
** MGCT: <option type="list">
** <item name="FLAG_ABORT_CONNECTION" value="FLAG_ABORT_CONNECTION"/>
** <item name="FLAG_CLOSE_TX" value="FLAG_CLOSE_TX"/>
** </option>
*/
#ifndef FTPDCFG_DATA_SHUTDOWN_OPTION
   #define FTPDCFG_DATA_SHUTDOWN_OPTION FLAG_CLOSE_TX  
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPDCFG_USES_MFS
   #if RTCS_MINIMUM_FOOTPRINT
      #define FTPDCFG_USES_MFS 0 
   #else
      #define FTPDCFG_USES_MFS 1
   #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPDCFG_ENABLE_MULTIPLE_CLIENTS
    #if RTCS_MINIMUM_FOOTPRINT
        #define FTPDCFG_ENABLE_MULTIPLE_CLIENTS 0
    #else
        #define FTPDCFG_ENABLE_MULTIPLE_CLIENTS 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
    #if RTCS_MINIMUM_FOOTPRINT
        #define FTPDCFG_ENABLE_USERNAME_AND_PASSWORD 0
    #else
        #define FTPDCFG_ENABLE_USERNAME_AND_PASSWORD 1
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPDCFG_ENABLE_RENAME
   #define FTPDCFG_ENABLE_RENAME 1//!RTCS_MINIMUM_FOOTPRINT 
#endif

/*
** Should be a multiple of maximum packet size
** Override in application by setting FTPd_window_size
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef FTPDCFG_WINDOW_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define FTPDCFG_WINDOW_SIZE 536 
   #else
      #define FTPDCFG_WINDOW_SIZE (6*1460)  
   #endif
#endif

/*
** Should be a multiple of sector size
** Override in application by setting FTPd_buffer_size
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef FTPDCFG_BUFFER_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define FTPDCFG_BUFFER_SIZE 512 
   #else
      #define FTPDCFG_BUFFER_SIZE 2048  
   #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef FTPDCFG_CONNECT_TIMEOUT
   #define FTPDCFG_CONNECT_TIMEOUT 1000 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef FTPDCFG_SEND_TIMEOUT
   #define FTPDCFG_SEND_TIMEOUT 5000 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef FTPDCFG_TIMEWAIT_TIMEOUT
   #define FTPDCFG_TIMEWAIT_TIMEOUT 500 
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** Telnet Configuration options
**
** MGCT: <category name="Telnet Configuration Options">
**
*/

/*
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef TELNETDCFG_BUFFER_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define TELNETDCFG_BUFFER_SIZE 256 
   #else
      #define TELNETDCFG_BUFFER_SIZE (3*1460)  
   #endif
#endif

/*
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef TELNETDCFG_NOWAIT
   #define TELNETDCFG_NOWAIT FALSE 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef TELNETDCFG_ENABLE_MULTIPLE_CLIENTS
    #if RTCS_MINIMUM_FOOTPRINT
        #define TELNETDCFG_ENABLE_MULTIPLE_CLIENTS 0
    #else
        #define TELNETDCFG_ENABLE_MULTIPLE_CLIENTS 1
    #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef TELENETDCFG_CONNECT_TIMEOUT
   #define TELENETDCFG_CONNECT_TIMEOUT 1000 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef TELENETDCFG_SEND_TIMEOUT
   #define TELENETDCFG_SEND_TIMEOUT 5000 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef TELENETDCFG_TIMEWAIT_TIMEOUT
   #define TELENETDCFG_TIMEWAIT_TIMEOUT 1000 
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** SNMP Configuration options
**
** MGCT: <category name="SNMP Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_SNMP_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_SNMP_STATS 1
    #else
        #define RTCSCFG_ENABLE_SNMP_STATS 0
    #endif
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** PPPHDLC Message Configuration options
**
** MGCT: <category name="PPPHDLC Message Configuration Options">
**
*/

/*
** MGCT: <option type="number"/>
*/
#ifndef PPPHDLC_INIT
   #if RTCS_MINIMUM_FOOTPRINT
      #define PPPHDLC_INIT 1 
   #else
      #define PPPHDLC_INIT 8
   #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef PPPHDLC_GROW
   #if RTCS_MINIMUM_FOOTPRINT
      #define PPPHDLC_GROW 1 
   #else
      #define PPPHDLC_GROW 0
   #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef PPPHDLC_MAX
   #if RTCS_MINIMUM_FOOTPRINT
      #define PPPHDLC_MAX 6 //2
   #else
      #define PPPHDLC_MAX 8
   #endif
#endif


/* MGCT: </category> */

/*
** MGCT: <category name="HTTPD Configuration Options">
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef HTTPDCFG_POLL_MODE
    #if RTCS_MINIMUM_FOOTPRINT
        #define HTTPDCFG_POLL_MODE              1
    #else
        #define HTTPDCFG_POLL_MODE              0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef HTTPDCFG_STATIC_TASKS
#define HTTPDCFG_STATIC_TASKS           0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef HTTPDCFG_DYNAMIC_TASKS
    #if RTCS_MINIMUM_FOOTPRINT
        #define HTTPDCFG_DYNAMIC_TASKS          0
    #else
        #define HTTPDCFG_DYNAMIC_TASKS          1
    #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_DEF_SERVER_PRIO
#define HTTPDCFG_DEF_SERVER_PRIO    (8)
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_DEF_SESSION_PRIO
#define HTTPDCFG_DEF_SESSION_PRIO   (8)
#endif

/*
** Default listen address
** MGCT: <option type="string"/>
*/
#ifndef HTTPDCFG_DEF_ADDR
#define HTTPDCFG_DEF_ADDR               INADDR_ANY
#endif

/*
** Default listen port
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_DEF_PORT
#define HTTPDCFG_DEF_PORT               80
#endif

/*
** MGCT: <option type="string" quoted="true"/>
*/
#ifndef HTTPDCFG_DEF_INDEX_PAGE
#define HTTPDCFG_DEF_INDEX_PAGE         "index.htm"
#endif

/*
** Default sessions count
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_DEF_SES_CNT
#define HTTPDCFG_DEF_SES_CNT            2 
#endif

/*
** Maximal URL length
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_DEF_URL_LEN
#define HTTPDCFG_DEF_URL_LEN            128 
#endif

/*
** Maximal length for auth data
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_DEF_AUTH_LEN
#define HTTPDCFG_DEF_AUTH_LEN           16
#endif

/*
** Maximal send data size in one step
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_MAX_BYTES_TO_SEND
#define HTTPDCFG_MAX_BYTES_TO_SEND      (512)
#endif

/*
** Maximal length for script line
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_MAX_SCRIPT_LN
#define HTTPDCFG_MAX_SCRIPT_LN          16
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_RECV_BUF_LEN
#define HTTPDCFG_RECV_BUF_LEN           32
#endif

/* 
** Default buffer configuration
** MGCT: <option type="number"/>
*/
#ifndef HTTPD_MAX_LEN
#define HTTPD_MAX_LEN                   128
#endif

/*
** Maximal length for http header
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_MAX_HEADER_LEN
#define HTTPDCFG_MAX_HEADER_LEN         256
#endif

/*
** Session timeout in milliseconds
** MGCT: <option type="number"/>
*/
#ifndef HTTPDCFG_SES_TO
#define HTTPDCFG_SES_TO                 (20000)
#endif

/*
** Request Timeout in milliseconds
** MGCT: <option type="number"/>
*/
#ifndef HTTPD_TIMEOUT_REQ_MS
#define HTTPD_TIMEOUT_REQ_MS            (4000)
#endif

/*
** Send Timeout in milliseconds
** MGCT: <option type="number"/>
*/
#ifndef HTTPD_TIMEOUT_SEND_MS
#define HTTPD_TIMEOUT_SEND_MS           (8000)
#endif

/* 
** Socket settings
** MGCT: <option type="string"/>
*/
#ifndef HTTPCFG_TX_WINDOW_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define HTTPCFG_TX_WINDOW_SIZE    (1460) 
   #else
      #define HTTPCFG_TX_WINDOW_SIZE    (3*1460) 
   #endif
#endif

/* 
** Socket settings
** MGCT: <option type="string"/>
*/
#ifndef HTTPCFG_RX_WINDOW_SIZE
   #if RTCS_MINIMUM_FOOTPRINT
      #define HTTPCFG_RX_WINDOW_SIZE    (1460) 
   #else
      #define HTTPCFG_RX_WINDOW_SIZE    (3*1460) 
   #endif
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef HTTPCFG_TIMEWAIT_TIMEOUT
   #define HTTPCFG_TIMEWAIT_TIMEOUT     1000 
#endif

/* 
** Default inet family configuration
** MGCT: <option type="string"/>
*/
#ifndef HTTPDCFG_AF
   #define HTTPDCFG_AF        			AF_INET
#endif


/* MGCT: </category> */
#endif


/*
** MGCT: <category name="IPv6 Specific Options">
*/

/* Multicast MUST be enabled for IPv6.*/
#if RTCSCFG_ENABLE_IP6
   #undef BSPCFG_ENABLE_ENET_MULTICAST
   #define BSPCFG_ENABLE_ENET_MULTICAST            (1)
#endif //RTCSCFG_ENABLE_IP6

/*
** Maximum number of entries in neighbor cache.
** NOTE: A small cache may result in an
** excessive number of Neighbor Discovery messages if entries are
** discarded and rebuilt in quick succession.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ND6_NEIGHBOR_CACHE_SIZE
   #define RTCSCFG_ND6_NEIGHBOR_CACHE_SIZE      (5) 
#endif

/*
** Maximum number of entries in the Prefix list (per interface).
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ND6_PREFIX_LIST_SIZE
    #define RTCSCFG_ND6_PREFIX_LIST_SIZE        (4) 
#endif


/*
** Maximum number of entries in the Default Router list (per interface).
** RFC4861: A host MUST retain at least two router
** addresses and SHOULD retain more.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ND6_ROUTER_LIST_SIZE
    #define RTCSCFG_ND6_ROUTER_LIST_SIZE        (2)
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP6_REASSEMBLY
   #define RTCSCFG_ENABLE_IP6_REASSEMBLY        1 
#endif

/*
** Maximum number of IPv6 addresses per interface.
** MGCT: <option type="number"/>
*/
#define RTCSCFG_IP6_IF_ADDRESSES_MAX            (5) 

#if RTCSCFG_IP6_IF_ADDRESSES_MAX < 2                                                 
    #undef RTCSCFG_IP6_IF_ADDRESSES_MAX
    #define RTCSCFG_IP6_IF_ADDRESSES_MAX        (2)
#endif

/*
** Loopback of own multicast packets.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IP6_LOOPBACK_MULTICAST
    #define RTCSCFG_IP6_LOOPBACK_MULTICAST      (0)
#endif


/* Check minimum values.*/
#if RTCSCFG_ND6_ROUTER_LIST_SIZE < 1 
    #error RTCSCFG_ND6_ROUTER_LIST_SIZE should be > 0
#endif
#if RTCSCFG_ND6_NEIGHBOR_CACHE_SIZE  < 1
    #error RTCSCFG_ND6_NEIGHBOR_CACHE_SIZE should be > 0
#endif
#if RTCSCFG_ND6_PREFIX_LIST_SIZE < 1 
    #error RTCSCFG_ND6_PREFIX_LIST_SIZE should be > 0
#endif

/*
** The number of consecutive Neighbor
** Solicitation messages sent while performing Duplicate Address
** Detection on a tentative address. A value of zero indicates that
** Duplicate Address Detection is not performed on tentative
** addresses. A value of one indicates a single transmission with no
** follow-up retransmissions.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ND6_DAD_TRANSMITS 
    #define RTCSCFG_ND6_DAD_TRANSMITS           (1) /* If RTCSCFG_ND6_DAD_TRANSMITS = 0, the DAD is disabled.*/
#endif    


/* For Debug usage only.*/
#ifndef RTCSCFG_DEBUG_TRACE_ETH
   #define RTCSCFG_DEBUG_TRACE_ETH              (0)
#endif
#ifndef RTCSCFG_DEBUG_TRACE_IP6
   #define RTCSCFG_DEBUG_TRACE_IP6              (0)
#endif
#ifndef RTCSCFG_DEBUG_TRACE_ICMP6
   #define RTCSCFG_DEBUG_TRACE_ICMP6            (0)
#endif
#ifndef RTCSCFG_DEBUG_MSG
   #define RTCSCFG_DEBUG_MSG                    (0)
#endif

/* MGCT: </category> */


/* EOF */
