#ifndef __nat_prv_h__
#define __nat_prv_h__
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
* $FileName: nat_prv.h$
* $Version : 3.8.7.0$
* $Date    : Oct-9-2012$
*
* Comments:
*
*   Private Network Address Translator definitions.
*
*END************************************************************************/

 
#include <tcpip.h>
#include <ip.h>
#include <ip_prv.h>
#include <icmp_prv.h>
#include <tcp_prv.h>
#include <udp_prv.h>
#include "nat_session.h"


/****************************************************
**
** CONSTANTS
*/

#define NAT_SESSION_INITIAL_COUNT   16
#define NAT_SESSION_GROW_COUNT      16
#define NAT_SESSION_MAX             0 

#define NAT_STATE_NORMAL            0x00
#define NAT_STATE_INSERT_IN         0x01
#define NAT_STATE_INSERT_OUT        0x02
#define NAT_STATE_FIN_ONE           0x04
#define NAT_STATE_FIN_TWO           0x08
#define NAT_STATE_ICMP_DEL          0x10
#define NAT_STATE_INS_ERR           0x20

#define NAT_ALG_TFTP_TYPE           0x0000
#define NAT_ALG_FTP_TYPE            0x0001

#define NAT_IS_PRIVATE   0x01
#define NAT_IS_LOCAL   0x02

#define NAT_PUBLIC         (0)
#define NAT_PRIVATE        (NAT_IS_PRIVATE)
#define NAT_PUBLIC_LOCAL   (NAT_IS_LOCAL)
#define NAT_PRIVATE_LOCAL  (NAT_IS_PRIVATE|NAT_IS_LOCAL)

#define NAT_DIRECTION(s,d) ((s<<2)|d)

#define NAT_PUBLIC_TO_PUBLIC                 NAT_DIRECTION(NAT_PUBLIC,NAT_PUBLIC)
#define NAT_PUBLIC_TO_PRIVATE                NAT_DIRECTION(NAT_PUBLIC,NAT_PRIVATE)
#define NAT_PUBLIC_TO_PUBLIC_LOCAL           NAT_DIRECTION(NAT_PUBLIC,NAT_PUBLIC_LOCAL)
#define NAT_PUBLIC_TO_PRIVATE_LOCAL          NAT_DIRECTION(NAT_PUBLIC,NAT_PRIVATE_LOCAL)

#define NAT_PRIVATE_TO_PUBLIC                NAT_DIRECTION(NAT_PRIVATE,NAT_PUBLIC)
#define NAT_PRIVATE_TO_PRIVATE               NAT_DIRECTION(NAT_PRIVATE,NAT_PRIVATE)
#define NAT_PRIVATE_TO_PUBLIC_LOCAL          NAT_DIRECTION(NAT_PRIVATE,NAT_PUBLIC_LOCAL)
#define NAT_PRIVATE_TO_PRIVATE_LOCAL         NAT_DIRECTION(NAT_PRIVATE,NAT_PRIVATE_LOCAL)

#define NAT_PUBLIC_LOCAL_TO_PUBLIC           NAT_DIRECTION(NAT_PUBLIC_LOCAL,NAT_PUBLIC)
#define NAT_PUBLIC_LOCAL_TO_PRIVATE          NAT_DIRECTION(NAT_PUBLIC_LOCAL,NAT_PRIVATE)
#define NAT_PUBLIC_LOCAL_TO_PUBLIC_LOCAL     NAT_DIRECTION(NAT_PUBLIC_LOCAL,NAT_PUBLIC_LOCAL)
#define NAT_PUBLIC_LOCAL_TO_PRIVATE_LOCAL    NAT_DIRECTION(NAT_PUBLIC_LOCAL,NAT_PRIVATE_LOCAL)

#define NAT_PRIVATE_LOCAL_TO_PUBLIC          NAT_DIRECTION(NAT_PRIVATE_LOCAL,NAT_PUBLIC)
#define NAT_PRIVATE_LOCAL_TO_PRIVATE         NAT_DIRECTION(NAT_PRIVATE_LOCAL,NAT_PRIVATE)
#define NAT_PRIVATE_LOCAL_TO_PUBLIC_LOCAL    NAT_DIRECTION(NAT_PRIVATE_LOCAL,NAT_PUBLIC_LOCAL)
#define NAT_PRIVATE_LOCAL_TO_PRIVATE_LOCAL   NAT_DIRECTION(NAT_PRIVATE_LOCAL,NAT_PRIVATE_LOCAL)



/***************************************
**
** MACROS
*/
#define TRANSPORT_PTR(ip_header_ptr)  ((uchar_ptr)ip_header_ptr) + IPH_LEN(ip_header_ptr);
#define NAT_KEY(prot,port)  (prot & 0xFF) << 16 | (port & 0xFFFF)
#define NAT_NEXT_PORT(c,p) ((p<=c->PORT_MIN?c->PORT_MAX:p-1))

#define NAT_UPDATE_ERR_STATS(c,priv)    if (priv) c->STATS.ST_PACKETS_PRV_PUB_ERR++; else c->STATS.ST_PACKETS_PUB_PRV_ERR++


/* 
** NAT_PARM:
**    NAT initialisation parameters.
*/
typedef struct nat_parm {
   TCPIP_PARM        COMMON;
   _ip_address       IP_PRV;
   _ip_address       IP_MASK;
   uint_32_ptr       LEN_PTR;
   uint_32           OPTION;
   pointer           CONFIG;
} NAT_PARM, _PTR_ NAT_PARM_PTR;


/*
** NAT_EVENT_HEAD:
**    Holds information about a NAT event queue
*/
typedef struct nat_event_head {
   NAT_EVENT_STRUCT_PTR    FIRST;         /* Points to first node in list */
   NAT_EVENT_STRUCT_PTR    LAST;          /* Points to last node in list */
   uint_32                 TIMEDELTA;     /* Time for last node to expire */
   uint_32                 TIMESTAMP;     /* Last modification time       */
} NAT_EVENT_HEAD, _PTR_ NAT_EVENT_HEAD_PTR;

/*
** NAT_ALG_CFG_STRUCT:
**    Holds NAT ALG specific information. 
*/
typedef uint_32 (_CODE_PTR_ NAT_ALG_APPLY_FUNC)(RTCSPCB_PTR _PTR_, boolean, 
   pointer, pointer _PTR_);

typedef struct nat_alg_cfg_struct {
   pointer              NEXT;
   uint_32              ALG_TYPE;
   NAT_ALG_APPLY_FUNC   ALG_EXEC;
} NAT_ALG_CFG_STRUCT, _PTR_ NAT_ALG_CFG_STRUCT_PTR;



/*
** DNAT_RULE_STRUCT:
**    DNAT rule information.
*/
typedef struct dnat_rule_struct {
   uint_32           PRIORITY;
   _ip_address       PRIVATE_IP;
   uint_16           IP_PROTOCOL;
   uint_16           PUBLIC_START_PORT;
   uint_16           PUBLIC_END_PORT;
   uint_16           PRIVATE_START_PORT;
   uint_16           PRIVATE_END_PORT;
   uint_16           RESERVED;
   uint_32         TIMEOUT;              
} DNAT_RULE_STRUCT, _PTR_ DNAT_RULE_STRUCT_PTR;

/*
** DNAT_PARM:
**    DNAT initialisation parameters.
*/
typedef struct dnat_parm {
   TCPIP_PARM          COMMON;
   DNAT_RULE_STRUCT  RULE;
} DNAT_PARM, _PTR_ DNAT_PARM_PTR;

/*
** DNAT_ELEMENT_STRUCT:
**    DNAT rule list element.
*/
typedef struct dnat_element_struct {
   QUEUE_ELEMENT_STRUCT    ELEMENT;
   DNAT_RULE_STRUCT         RULE;
} DNAT_ELEMENT_STRUCT, _PTR_ DNAT_ELEMENT_STRUCT_PTR;


/*
** NAT_CFG_STRUCT:
**    Holds NAT configuration information. 
*/
typedef struct nat_cfg_struct {
   uint_32 (_CODE_PTR_     NAT_EXEC)(RTCSPCB_PTR _PTR_); /* PCB with packet */
   _ip_address             INITIAL_PRV_NET;          /* initial private network - for EDS compatibility */
   _ip_address             INITIAL_PRV_MASK;         /* initial private network mask - for EDS compatibility */
   IPRADIX_NODE            ROOT_IN;                  /* incoming packet tree     */
   IPRADIX_NODE            ROOT_OUT;                 /* outgoing packet tree     */
   _rtcs_part              SESSION_PART;             /* session partition        */
   _rtcs_part              RADIX_IN;                 /* for incoming packets     */
   _rtcs_part              RADIX_OUT;                /* for outgoing packets     */
   NAT_ALG_CFG_STRUCT_PTR  ALG_INFO_PTR;             /* ALG specific config      */
   uint_16                 TCP_PORT;                 /* last used TCP port #     */
   uint_16                 UDP_PORT;                 /* last used UDP port #     */   
   uint_16                 ICMP_ID;                  /* last used ICMP ID        */   
   uint_16                 GLOBAL_PORT;             
   TCPIP_EVENT             TCP_TOUT;                 /* TCP timeout event        */
   TCPIP_EVENT             UDP_TOUT;                 /* UDP timeout event        */
   TCPIP_EVENT             FIN_TOUT;                 /* FIN timeout event        */
   NAT_EVENT_HEAD          TCP_HEAD;                 /* TCP Timeout event heads  */
   NAT_EVENT_HEAD          UDP_HEAD;                 /* UDP Timeout event heads  */
   NAT_EVENT_HEAD          FIN_HEAD;                 /* FIN Timeout event heads  */
   NAT_STATS               STATS;                    /* NAT stats                */
   uint_32                 TIMEOUT_TCP;              /* TCP session tout         */
   uint_32                 TIMEOUT_UDP;              /* UDP session tout         */
   uint_32                 TIMEOUT_FIN;              /* FIN-RST session tout     */
   uint_16                 PORT_MIN;                 /* Min used port number     */
   uint_16                 PORT_MAX;                 /* Max used port number     */

   TCPIP_EVENT             ICMP_TOUT;                /* ICMP timeout event       */
   uint_32                 TIMEOUT_ICMP;             /* ICMP session tout        */
   NAT_EVENT_HEAD          ICMP_HEAD;                /* ICMP Timeout event heads */
   QUEUE_STRUCT            RULE_QUEUE;
   NAT_NETWORK_STRUCT      PRIVATE_NETWORKS;         /* The private networks     */
} NAT_CFG_STRUCT, _PTR_ NAT_CFG_STRUCT_PTR;


/*
** TRANSPORT_UNION:
**    To reference the various possible transport payloads. 
*/
typedef union {
   pointer                 PTR;
   TCP_HEADER_PTR          TCP_PTR;
   UDP_HEADER_PTR          UDP_PTR;
   ICMP_HEADER_PTR         ICMP_PTR;
   ICMP_ECHO_HEADER_PTR    ECHO_PTR;
   ICMP_ERR_HEADER_PTR     ERR_PTR;
   uchar_ptr               DATA_OFFSET;
} TRANSPORT_UNION; 




/***************************************
**
** Function definitions
**
*/
extern void NAT_init_internal
(
   NAT_PARM_PTR      /* [IN] Initialization parameters */
);

void NAT_add_network_internal
(
   NAT_PARM_PTR      /* [IN] Network parameters */
);

void NAT_remove_network_internal
(
   NAT_PARM_PTR      /* [IN] Network parameters */
);

extern void NAT_close_internal
(
   NAT_PARM_PTR
);

extern NAT_SESSION_STRUCT_PTR NAT_lookup
(
   IP_HEADER_PTR,          /* [IN] The IP datagram */
   boolean,                /* [IN] Is it for an icmp error? */
   boolean,                /* [IN] Non strict lookup ? */
   uint_32_ptr             /* [OUT] Reason for returning NULL */
);
   
extern NAT_SESSION_STRUCT_PTR NAT_insert
(
   IP_HEADER_PTR,          /* [IN] Pointer to the IP datagram */
   uint_32_ptr             /* [OUT] Reason for returning NULL */
);

extern NAT_SESSION_STRUCT_PTR NAT_insert_dnat
(
   IP_HEADER_PTR,          /* [IN] Pointer to the IP datagram */
   DNAT_RULE_STRUCT_PTR,   /* [IN} DNAT Rule                  */
   uint_32_ptr             /* [OUT] Reason for returning NULL */
);

extern void NAT_delete
(
   NAT_SESSION_STRUCT_PTR  /* [IN] The session structure */
);
   
extern void NAT_find_next_session_internal
(
   NAT_PARM_PTR
);

extern uint_32 NAT_apply
(
   RTCSPCB_PTR _PTR_       /* [IN/OUT] PCB containing the packet */
 );

extern boolean NAT_expire
(
   TCPIP_EVENT_PTR         /* [IN] Points to the event that expired */
);

extern void NAT_event_add
(
   TCPIP_EVENT_PTR,        /* [IN] Controller event */
   NAT_EVENT_STRUCT_PTR    /* [IN] NAT event to add */
);

extern void NAT_event_del
(
   NAT_EVENT_STRUCT_PTR    /* [IN] NAT event to del */
);


extern void NAT_event_tick
(
   TCPIP_EVENT_PTR,        /* [IN] Controlling event           */
   boolean                 /* [IN] Force expiry of first event */
);

extern void NAT_tout_max
(
   TCPIP_EVENT_PTR,        /* [IN] Event queue to update */
   uint_32                 /* [IN] New maximum timeout for events in queue */
);

extern uint_16 NAT_chksum_mod
(
   uint_16  oldchk,        /* [IN] The old checksum */
   uint_16  old16,         /* [IN] The old data being replaced */
   uint_16  new16          /* [IN] The new replacement data */
);

extern void NAT_mod_ip
(
   IP_HEADER_PTR,          /* [IN/OUT] IP datagram */
   NAT_SESSION_STRUCT_PTR, /* [IN]     NAT session */
   boolean                 /* [IN]     Direction   */
);

extern void NAT_mod_tcpudp
(
   TRANSPORT_UNION,        /* [IN/OUT] Transport protocol header */ 
   NAT_SESSION_STRUCT_PTR, /* [IN] The session struct            */
   boolean,                /* [IN] Direction of packet           */
   boolean                 /* [IN] Will it go on the FINRST Q?   */
);

extern uint_32 NAT_mod_icmp
(
   IP_HEADER_PTR,                /* [IN/OUT] IP datagram       */
   NAT_SESSION_STRUCT_PTR _PTR_, /* [IN]     NAT session       */
   boolean,                      /* [IN]     Direction         */
   boolean _PTR_                 /* [OUT] DEST is NAT router?  */
);

extern void NAT_config_timeouts
(
   nat_timeouts _PTR_ config     /* [IN] New timeouts */
);

extern void NAT_config_ports
(
   nat_ports _PTR_ config        /* [IN] New port range */
);

extern void NAT_getopt  
(
   NAT_PARM_PTR                  /* [IN/OUT] options */
);

extern void NAT_setopt  
(
   NAT_PARM_PTR                  /* [IN/OUT] options */
);

extern boolean NAT_is_private_addr
(
   NAT_NETWORK_STRUCT_PTR, 
   _ip_address
);

extern uint_32 NAT_add_private_network
(
   NAT_NETWORK_STRUCT_PTR, 
   _ip_address, 
   _ip_address
);

extern uint_32 NAT_remove_private_network(
   NAT_NETWORK_STRUCT_PTR, 
   _ip_address, 
   _ip_address
);

/***************************************
**
** Function definitions for DNAT
**
*/

extern DNAT_RULE_STRUCT_PTR DNAT_lookup_rule
(
   NAT_CFG_STRUCT_PTR,
   IP_HEADER_PTR,              /* [IN] The IP datagram */
   boolean
);

extern void DNAT_add_rule_internal
(
   DNAT_PARM_PTR                   /* [IN] DNAT rule parameters */
);

extern void DNAT_delete_rule_internal
(
   DNAT_PARM_PTR                   /* [IN] DNAT rule priority */ 
);

extern void DNAT_get_next_rule_internal
(
   DNAT_PARM_PTR                   /* [IN] DNAT rule priority */ 
);


/***********************
**
** NAT ALG prototypes
*/

typedef void (_CODE_PTR_ NAT_ALG_FREE_FUNC)
(
   pointer
);

extern NAT_ALG_FREE_FUNC NAT_alg_free_func_table[];

extern uint_32 NAT_init_algs
(
   NAT_CFG_STRUCT_PTR
);

extern void NAT_ALG_TCP_checksum
(
   IP_HEADER_PTR
);

extern uint_32 NAT_ALG_tftp
(
   pointer
);

extern uint_32 NAT_ALG_tftp_apply
(
   RTCSPCB_PTR _PTR_, 
   boolean, 
   pointer, 
   pointer _PTR_
);

extern boolean NAT_ALG_tftp_lookup
(
   RTCSPCB_PTR _PTR_
);

extern uint_32 NAT_ALG_ftp
(
   pointer
);

extern uint_32 NAT_ALG_ftp_apply
(
   RTCSPCB_PTR _PTR_, 
   boolean, 
   pointer, 
   pointer _PTR_
);

#endif
