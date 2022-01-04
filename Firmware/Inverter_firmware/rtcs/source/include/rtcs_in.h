#ifndef __rtcs_in_h__
#define __rtcs_in_h__
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
* $FileName: rtcs_in.h$
* $Version : 3.0.6.0$
* $Date    : Apr-24-2009$
*
* Comments:
*
*   Definitions for the Internet protocol layer.
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/

/*
** IP protocol types
*/
#define IPPROTO_HOPOPTS    0        /* IPv6 hop-by-hop options            */
#define IPPROTO_ICMP       1        /* Internet Control Message Protocol  */
#define IPPROTO_IGMP       2        /* Internet Group Management Protocol */
#define IPPROTO_IP         4        /* IP-in-IP encapsulation             */
#define IPPROTO_IPIP       4        /* IP-in-IP encapsulation             */
#define IPPROTO_TCP        6        /* Transmission Control Protocol      */
#define IPPROTO_UDP        17       /* User Datagram Protocol             */
#define IPPROTO_IPV6       41       /* IPv6-in-IP encapsulation           */
#define IPPROTO_ROUTING    43       /* IPv6 routing header                */
#define IPPROTO_FRAGMENT   44       /* IPv6 fragmentation header          */
#define IPPROTO_ESP        50       /* Encapsulating Security Payload     */
#define IPPROTO_AH         51       /* Authentication Header              */
#define IPPROTO_ICMPV6     58       /* ICMPv6                             */
#define IPPROTO_NONE       59       /* IPv6 no next header                */
#define IPPROTO_DSTOPTS    60       /* IPv6 destination options           */
#define IPPROTO_OSPF       89       /* Open Shortest Path Protocol        */
#define IPPROTO_COMP       108      /* IP compression                     */

/*
** Standard port assignments
*/
#define IPPORT_SPR_ITUNES  0
#define IPPORT_TCPMUX      1
#define IPPORT_COMPRESSNET 2
#define IPPORT_RJE         5
#define IPPORT_ECHO        7
#define IPPORT_DISCARD     9
#define IPPORT_SYSSTAT     11
#define IPPORT_DAYTIME     13
#define IPPORT_NETSTAT     15
#define IPPORT_QOTD        17
#define IPPORT_MSP         18
#define IPPORT_CHARGEN     19
#define IPPORT_FTPDATA     20
#define IPPORT_FTP         21
#define IPPORT_SSH         22
#define IPPORT_TELNET      23
#define IPPORT_SMTP        25
#define IPPORT_NEWS_FE     27
#define IPPORT_MSG_ICP     29
#define IPPORT_MSG_AUTH    31
#define IPPORT_DSP         33
#define IPPORT_TIME        37
#define IPPORT_RLP         39
#define IPPORT_GRAPHICS    41
#define IPPORT_NAME        42

#define IPPORT_BOOTPS      67
#define IPPORT_BOOTPC      68
#define IPPORT_TFTP        69
#define IPPORT_HTTP        80
#define IPPORT_NTP         123
#define IPPORT_SNMP        161
#define IPPORT_SNMPTRAP    162
#define IPPORT_ISAKMP      500
#define IPPORT_WHO         513
#define IPPORT_RIP         520
#define IPPORT_ESPUDP      4500

/*
** IANA ifTypes
*/
#define IPIFTYPE_OTHER           1
#define IPIFTYPE_ETHERNET        6
#define IPIFTYPE_PPP             23
#define IPIFTYPE_LOOPBACK        24
#define IPIFTYPE_RS232           33
#define IPIFTYPE_FASTETHERNET    62
#define IPIFTYPE_GIGABITETHERNET 117
#define IPIFTYPE_HDLC            118
#define IPIFTYPE_TUNNEL          131

/*
** IANA ARP hardware types
*/
#define ARPLINK_NONE       0
#define ARPLINK_ETHERNET   1
#define ARPLINK_SERIAL     20

/*
** Special IP addresses
*/
#define INADDR_ANY               0
#define INADDR_LOOPBACK          0x7F000001L
#define INADDR_ALLHOSTS_GROUP    0xE0000001L
#define INADDR_ALLROUTERS_GROUP  0xE0000002L
#define INADDR_RIP_GROUP         0xE0000009L
#define INADDR_NTP_GROUP         0xE0000101L
#define INADDR_BROADCAST         0xFFFFFFFFL


/***************************************
**
** Code macros
**
*/

/*
** Macros to classify IP addresses:
*/
#define IN_ZERONET(a)         (((a) & 0xFF000000L) == 0x00000000L)
#define IN_LOOPBACK(a)        (((a) & 0xFF000000L) == 0x7F000000L)
#define IN_MULTICAST(a)       (((a) & 0xF0000000L) == 0xE0000000L)
#define IN_LOCAL_MULTICAST(a) (((a) & 0xFFFFFF00L) == 0xE0000000L)
#define IN_EXPERIMENTAL(a)    (((a) & 0xF0000000L) == 0xF0000000L)

#define IN_CLASSA(a)          (((a) & 0x80000000L) == 0x00000000L)
#define IN_CLASSA_NET         0xFF000000L
#define IN_CLASSB(a)          (((a) & 0xC0000000L) == 0x80000000L)
#define IN_CLASSB_NET         0xFFFF0000L
#define IN_CLASSC(a)          (((a) & 0xE0000000L) == 0xC0000000L)
#define IN_CLASSC_NET         0xFFFFFF00L

#define IN_DEFAULT_NET(a)  ((a) == INADDR_ANY ? INADDR_ANY : \
                            (IN_CLASSA(a) ? IN_CLASSA_NET :  \
                            (IN_CLASSB(a) ? IN_CLASSB_NET :  \
                            (               IN_CLASSC_NET))))


#endif
/* EOF */
