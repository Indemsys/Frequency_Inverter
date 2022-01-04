#ifndef __nat_h__
#define __nat_h__
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
* $FileName: nat.h$
* $Version : 3.6.4.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   Network Address Translator definitions.
*
*END************************************************************************/


#if RTCS_VERSION < 0x00029600L
#error "This version of NAT requires RTCS 2.96.00 or higher"
#endif


/*************************************
**
** NAT constants
*/
#define NAT_DEFAULT_TIMEOUT_TCP     (15 * 60 * 1000)  /* 15 minutes */
#define NAT_DEFAULT_TIMEOUT_UDP     ( 5 * 60 * 1000)  /*  5 minutes */
#define NAT_DEFAULT_TIMEOUT_FIN     ( 2 * 60 * 1000)  /*  2 minutes */
/*
 * ICMP specific NAT extension
 * NAT Default ICMP session timeout. It should be small in order
 * to minimized the system resources, each PING creates a new NAT
 * session
 */
#define NAT_DEFAULT_TIMEOUT_ICMP    ( 1 * 60 * 1000)  /*  1 minutes */

// IANA Port Ranges
#define NAT_WELL_KNOWN_PORT_MIN     0
#define NAT_WELL_KNOWN_PORT_MAX     1023
#define NAT_REGISTERED_PORT_MIN     1024
#define NAT_REGISTERED_PORT_MAX     49151
#define NAT_DYNAMIC_PORT_MIN        49152
#define NAT_DYNAMIC_PORT_MAX        65535

#define NAT_DEFAULT_PORT_MIN        10000
#define NAT_DEFAULT_PORT_MAX        20000

#define SNAT 0 
#define DNAT 1 

#define NAT_MAX_PRIVATE_NETWORKS    16    // maximum number of supported private networks


/**************************************
**
** NAT setsockopt 
*/

extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_NAT_CALL;




/* 
** NAT Timeout structure (for NAT configuration)
*/
typedef struct nat_timeouts_struct {
   uint_32  timeout_tcp;
   uint_32  timeout_fin;
   uint_32  timeout_udp;
   uint_32  timeout_icmp;     // NAT ICMP session timeout extension 
} nat_timeouts, NAT_TIMEOUTS_STRUCT, _PTR_ NAT_TIMEOUTS_STRUCT_PTR;


/* 
** NAT Port structure (for NAT configuration)
*/
typedef struct nat_ports_struct {
   uint_16  port_min;
   uint_16  port_max;
} nat_ports, NAT_PORTS_STRUCT, _PTR_ NAT_PORTS_STRUCT_PTR;


/* 
** NAT_STATS:
**    A structure used for statistics 
*/
typedef struct nat_stats {
   uint_32  ST_SESSIONS;            /* total amount of sessions created to date */
   uint_32  ST_SESSIONS_OPEN;       /* amount of sessions currently open */
   uint_32  ST_SESSIONS_OPEN_MAX;   /* max amount of sessions simult. open */   
   uint_32  ST_PACKETS_TOTAL;       /* amount of packets processed by NAT */
   uint_32  ST_PACKETS_BYPASS;      /* amount of unmodified packets */
   uint_32  ST_PACKETS_PUB_PRV;     /* packets from public to private */
   uint_32  ST_PACKETS_PUB_PRV_ERR; /* packets from public to private with errs */
   uint_32  ST_PACKETS_PRV_PUB;     /* packets from private to public */
   uint_32  ST_PACKETS_PRV_PUB_ERR; /* packets from private to public with errs */

   uint_32  ST_PACKETS_FRAGMENTS;
   uint_32  ST_PACKETS_REASSEMBLED;
   uint_32  ST_PACKETS_REASSSEMBLY_ERR;

   uint_32  ST_SESSIONS_SNAT;       /* total amount of SNAT sessions created to date */
   uint_32  ST_SESSIONS_DNAT;       /* total amount of DNAT sessions created to date */

} NAT_STATS, _PTR_ NAT_STATS_PTR;


typedef struct nat_network_struct {
   uint_32                 NUM_PRV_NETS;              /* Number of initialized private networks */
   _ip_address             PRV_NET[NAT_MAX_PRIVATE_NETWORKS];  /* List of private networks      */
   _ip_address             PRV_MASK[NAT_MAX_PRIVATE_NETWORKS]; /* Corresponding network masks   */
} NAT_NETWORK_STRUCT, _PTR_ NAT_NETWORK_STRUCT_PTR;

typedef uint_32 (_CODE_PTR_ NAT_ALG)(pointer);

/*
** NAT_alg_table:
**    NULL terminated table which specifies enabled ALGs 
*/

#define NAT_ALG_TFTP    NAT_ALG_tftp
#define NAT_ALG_FTP     NAT_ALG_ftp
#define NAT_ALG_ENDLIST NULL
extern NAT_ALG NAT_alg_table[];

/***************************************
**
** Function definitions
**
*/
extern uint_32 NAT_init
(
   _ip_address,         /* [IN] private network IP */
   _ip_address          /* [IN] private network netmask */
);

extern uint_32 NAT_add_network
(
   _ip_address    private_network,
   _ip_address    private_netmask
);

extern uint_32 NAT_remove_network
(
   _ip_address    private_network,
   _ip_address    private_netmask
);

extern uint_32 NAT_close
(  
   void
);

extern NAT_STATS_PTR NAT_stats
(
   void
);

extern NAT_NETWORK_STRUCT_PTR NAT_networks
(
   void
);

extern pointer NAT_find_next_session
(
   pointer  session,
   uint_32  tree
);

extern uint_32 NAT_ALG_tftp(pointer);
extern uint_32 NAT_ALG_ftp(pointer);

/***************************************
**
** Function definitions for DNAT
**
*/
extern uint_32 DNAT_add_rule
(
   uint_32,       /* [IN] */    
   uint_16,       /* [IN] */
   uint_16,       /* [IN] */
   uint_16,       /* [IN] */
   _ip_address,   /* [IN] */
   uint_16        /* [IN] */
);

extern uint_32 DNAT_delete_rule
(
      uint_32
);

extern uint_32  DNAT_get_next_rule
(
   uint_32_ptr,    /* [IN/OUT] */   
   uint_16_ptr,    /* [OUT */
   uint_16_ptr,    /* [OUT] */
   uint_16_ptr,     /* [OUT] */
   _ip_address _PTR_,/* [OUT]   */
   uint_16_ptr       /* [OUT] */
);

#endif

/* EOF */
