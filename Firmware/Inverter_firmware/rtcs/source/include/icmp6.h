#ifndef __icmp6_h__
#define __icmp6_h__
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
* $FileName: icmp6.h$
* $Version : 3.8.1.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Definitions for the ICMP6 protocol layer.
*
*END************************************************************************/


/******************************************************************
* ICMPv6 message types (RFC 4443)
******************************************************************/

/* ICMPv6 error messages:*/
#define ICMP6_TYPE_DEST_UNREACH   	    1	/* Destination Unreachable. */
#define ICMP6_TYPE_PACKET_TOOBIG  	    2	/* Packet Too Big. */
#define ICMP6_TYPE_TIME_EXCEED	    	3	/* Time Exceeded. */
#define ICMP6_TYPE_PARAM_PROB		    4	/* Parameter Problem. */

/* ICMPv6 informational messages:*/
#define ICMP6_TYPE_ECHO_REQ			    128 /* Echo Request. */
#define ICMP6_TYPE_ECHO_REPLY		    129	/* Echo Reply. */

/*  Neighbor Discovery defines five different ICMP packet types (RFC4861):*/
#define ICMP6_TYPE_ROUTER_SOLICITATION		133	/* Router Solicitation. */
#define ICMP6_TYPE_ROUTER_ADVERTISEMENT		134	/* Router Advertisement. */
#define ICMP6_TYPE_NEIGHBOR_SOLICITATION	135	/* Neighbor Solicitation. */
#define ICMP6_TYPE_NEIGHBOR_ADVERTISEMENT	136	/* Neighbor Advertisement. */
#define ICMP6_TYPE_REDIRECT             	137 /* Redirect.*/

/* Destination Unreachable codes */
#define ICMP6_CODE_DU_NO_ROUTE          0   /* No route to destination. */
#define ICMP6_CODE_DU_ADMIN_PROHIBITED  1   /* Communication with destination administratively prohibited. */
#define ICMP6_CODE_DU_BEYOND_SCOPE      2   /* Beyond scope of source address.*/
#define ICMP6_CODE_DU_ADDR_UNREACH      3   /* Address unreachable.*/
#define ICMP6_CODE_DU_PORT_UNREACH      4   /* Port unreachable.*/
#define ICMP6_CODE_DU_ADDR_FAILED       5   /* Source address failed ingress/egress policy.*/
#define ICMP6_CODE_DU_REJECT_ROUTE      6   /* Reject route to destination.*/

/* Packet Too Big codes */
#define ICMP6_CODE_PTB                  0  

/* Time Exceeded codes */
#define ICMP6_CODE_TE_HOP_LIMIT         0   /* Hop limit exceeded in transit.*/
#define ICMP6_CODE_TE_FRG_REASSEMBLY    1   /* Fragment reassembly time exceeded.*/

/* Parameter Problem codes */
#define ICMP6_CODE_PP_HEADER            0   /* Erroneous header field encountered.*/
#define ICMP6_CODE_PP_NEXT_HEADER       1   /* Unrecognized Next Header type encountered.*/
#define ICMP6_CODE_PP_OPTION            2   /* Unrecognized IPv6 option encountered.*/


/******************************************************************
 * ICMP echo parameters
 ******************************************************************/
typedef struct icmp6_parm
{
    TCPIP_PARM                      COMMON;
    struct icmp6_parm               * NEXT;
    struct icmp6_parm               * * PREV;
    TCPIP_EVENT                     EXPIRE;
    in6_addr                        *ip_addr;
    uint_32                         if_scope_id;        /* Interface number. */
    uint_32                         timeout;
    uint_16                         id;
    uint_16                         seq;
    uint_8                          hop_limit;          /* Hop limit.*/ 
    pointer                         data_buffer;        /* Pointer to a data buffer (optional). */
    uint_32                         data_buffer_size;   /* Size of a buffer (optional). */
} ICMP6_PARM, _PTR_ ICMP6_PARM_PTR;

/******************************************************************
* Function Prototypes
*******************************************************************/
extern uint_32 ICMP6_init ( void );
extern void ICMP6_service (RTCSPCB_PTR, pointer);
extern void ICMP6_send_echo (ICMP6_PARM_PTR);

void ICMP6_send_error ( uint_8 type     /* [IN] the ICMPv6 type to send */,    
                        uint_8 code     /* [IN] the ICMPv6 code to send */,
                        uint_32 param   /* [IN] error parameter */,
                        RTCSPCB_PTR origpcb   /* [IN] the packet which caused the error */ );

#endif /* __icmp6_h__ */

