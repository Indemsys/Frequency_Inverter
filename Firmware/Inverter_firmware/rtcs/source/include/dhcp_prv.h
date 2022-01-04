#ifndef __dhcp_prv_h__
#define __dhcp_prv_h__
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
* $FileName: dhcp_prv.h$
* $Version : 3.0.7.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   Dynamic Host Configuration Protocol definitions.
*
*END************************************************************************/

#include "ip_prv.h"



/***************************************
**
** Constants
**
*/

#define DHCP_TIMEOUT_MIN       4000     /* 4 sec */
#define DHCP_TIMEOUT_MAX       64000    /* 64 sec */
#define DHCP_SAVED_ARRAY_SIZE  4
#define DHCP_SITE_SPEC_MIN     128
#define DHCP_SITE_SPEC_MAX     254

/* Internal DHCP Error codes - backward compatibility */

#define DHCP_PACKET_ERROR          RTCSERR_DHCP_PACKET_ERROR
#define DHCPCLNT_ERROR_DECLINED    RTCSERR_DHCPCLNT_ERROR_DECLINED
#define DHCPCLNT_XID_MISMATCH      RTCSERR_DHCPCLNT_XID_MISMATCH
#define DHCPCLNT_PACKET_SIZE_ERROR RTCSERR_DHCPCLNT_PACKET_SIZE_ERROR

/*
** This value can actually be 0x418937 before a rollover occurs, but the
** Diab compiler treats it as signed, so an overflow warning occurs unless
** you use 0x20C49B.
*/
#define DHCP_TCPIP_MAX_TIMED_EVENT_VALUE  0x20C49B

/*
** Standard port assignments
*/

#define DHCP_SERVER_PORT            67
#define DHCP_CLIENT_PORT            68


/* DHCP State definitions  */

#define DHCP_INITIALIZE    1
#define DHCP_BOUND         2
#define DHCP_RENEWING      3
#define DHCP_REBINDING     4
#define DHCP_REQUESTING    5
#define DHCP_DECLINING     6
#define DHCP_RELEASING     7
#define DHCP_INFORMED      8


/* 5 percent of total lease, there's no standard, use whatever's appropriate */
#define DHCP_RENEWALL_RANGE         0x14

/* The renewall and rebinding biases are suggested in RFC1541 */
#define DHCP_RENEWALL_BIAS(T)     (T/2)
#define DHCP_REBINDING_BIAS(T)    (T * 7/8)

/* Wait ten seconds before re-trying */
#define DHCP_TERMINATION_RETRY       10000
#define DHCP_OFFER_CHECK_EXPIRY_TIME 3000

/***************************************
**
** DHCP Macros
**
*/

#define DHCP_OPTION(type,len,val) \
            htonc(optptr, DHCPOPT_  ## type); optptr++; optlen++; \
            htonc(optptr, DHCPSIZE_ ## type); optptr++; optlen++; \
            hton ## len(optptr, val);                             \
            optptr += DHCPSIZE_ ## type;                          \
            optlen += DHCPSIZE_ ## type

#define DHCP_FILL(opt, cond) \
            opt_ptr = DHCP_find_option(parms->OPT_BUFF,                        \
                                       parms->OPT_BUFF_SIZE, DHCPOPT_ ## opt); \
            if (opt_ptr) { len = ntohc(opt_ptr + 1) + 2;                       \
               if (cond) { _mem_copy(opt_ptr, outp, len); outp += len;}        \
               for (i = 0; i < len; i++) {htonc(opt_ptr, 0); opt_ptr++;}}

#define DHCP_SET_RENEWALL_RANGE(l, r) \
            r = l / DHCP_RENEWALL_RANGE


#define DHCPCLNT_TIME_OVERFLOW_CHECK(r, t) if ( t > DHCP_TCPIP_MAX_TIMED_EVENT_VALUE ) { \
            t -= DHCP_TCPIP_MAX_TIMED_EVENT_VALUE;                                       \
            r = DHCP_TCPIP_MAX_TIMED_EVENT_VALUE * 1000;} else {                         \
            r = t * 1000;                                                                \
            t = 0;}

/***************************************
**
** Type definitions
**
*/

/***************************************
**
** Function-specific type definitions
**
*/

/* DHCP_open() */
typedef struct tcpip_parm_if_dhcp {

   TCPIP_PARM           COMMON;
   _rtcs_if_handle      HANDLE;
   uint_32              FLAGS;

   uchar_ptr            OPT_BUFF;
   uint_32              OPT_BUFF_SIZE;
   DHCP_DATA_STRUCT_PTR OPTIONS;
   _ip_address          CLNT_IP_ADDR;
   _ip_address          CLNT_IP_MASK;
   _ip_address          SERVER_IP_ADDR;
   uint_32              LEASE;
   uint_32              TIMEOUT;

} TCPIP_PARM_IF_DHCP, _PTR_ TCPIP_PARM_IF_DHCP_PTR;

/* DHCPCLNT_release() */
typedef struct tcpip_parm_if_dhcp_release {

   TCPIP_PARM           COMMON;
   _rtcs_if_handle      HANDLE;

} TCPIP_PARM_IF_DHCP_RELEASE, _PTR_ TCPIP_PARM_IF_DHCP_RELEASE_PTR;

/* Kept for backwards compatibility, still use internally */
typedef struct dhcp_lease_struct 
{
   uint_32     LEASE;
   _ip_address ServerIp;
   _ip_address ClientIPaddress;
   _ip_address Netmask;
   _ip_address SADDR;
   uchar       SNAME[64];
   uchar       BOOTFILE[128];

} DHCP_LEASE_STRUCT, _PTR_ DHCP_LEASE_STRUCT_PTR;

typedef struct dhcp_stats {

   RTCS_STATS_STRUCT COMMON;

   RTCS_ERROR_STRUCT ERR_RX;
   RTCS_ERROR_STRUCT ERR_TX;

   uint_32 ST_TX_DISCOVER;
   uint_32 ST_TX_DECLINE;
   uint_32 ST_TX_REQUEST;
   uint_32 ST_RX_OFFER;
   uint_32 ST_RX_ACK;
   uint_32 ST_RX_NAK;

} DHCP_STATS, _PTR_ DHCP_STATS_PTR;

typedef struct dhcp_clnt_struct  {

   IP_IF            _PTR_ HANDLE;  /* [IN] the RTCS interface state structure */
   UCB_STRUCT       _PTR_ UCB;
   uchar            _PTR_ SERVERID_PTR;
   uchar            _PTR_ MSGTYPE_PTR;
   uchar            _PTR_ PACKET;
   uchar            _PTR_ REQUEST_OPT_PTR;
   DHCP_LEASE_STRUCT      DATA;    /* [IN/OUT] DHCP parameters for callback */
   DHCP_LEASE_STRUCT      NEW_DATA; /* [IN] DHCP parameters for initialization*/
   TCPIP_EVENT            RENEW;
   TCPIP_EVENT            RESEND;
   TCPIP_EVENT            TERMINATE;
   boolean                BOUND;
   uint_32                STATE;
   uint_32                INIT_TIMEOUT;
   uint_32                NEW_RENEW_TIME;
   uint_32                RENEW_TIME;
   uint_32                NEW_REBIND_TIME;
   uint_32                REBIND_TIME;
   uint_32                RETRY_TIMEOUT;
   uint_32                TERMINATE_TIME;
   uint_32                R_EVENTS_TIME_LEFT;
   uint_32                S_EVENT_TIME_LEFT;
   uint_32                SECS;
   uint_32                TOTAL_PACKET_SIZE;
   uint_32                CURRENT_PACKET_SIZE;
   uint_32                FLAGS;
   int_32                 (_CODE_PTR_ CHOICE_FUNC)(uchar _PTR_, uint_32);
   void                   (_CODE_PTR_ BIND_FUNC)  (uchar _PTR_, uint_32, _rtcs_if_handle);
   boolean                (_CODE_PTR_ UNBIND_FUNC)(_rtcs_if_handle);
   boolean                (_CODE_PTR_ FAILURE_FUNC)(_rtcs_if_handle);
   boolean                (_CODE_PTR_ NAK_FUNC)  (uchar _PTR_, uint_32, _rtcs_if_handle);
   DHCP_STATS             STATS;
   IPIF_PARM              PARMS_BIND;
   IPIF_PARM              NEW_PARMS_BIND;

} DHCP_CLNT_STRUCT, _PTR_ DHCP_CLNT_STRUCT_PTR;

/***************************************
**
** Prototypes
**
*/

void DHCPCLNT_init ( TCPIP_PARM_IF_DHCP _PTR_ );
void DHCPCLNT_reinit ( TCPIP_PARM_IF_DHCP _PTR_ );
void DHCPCLNT_service ( RTCSPCB _PTR_, UCB_STRUCT _PTR_);
boolean DHCPCLNT_send ( TCPIP_EVENT _PTR_ );

uint_32 DHCPCLNT_start ( TCPIP_PARM_IF_DHCP _PTR_, DHCP_CLNT_STRUCT _PTR_ );
void DHCPCLNT_release_internal(TCPIP_PARM_IF_DHCP_RELEASE _PTR_ );

#endif
/* EOF */
