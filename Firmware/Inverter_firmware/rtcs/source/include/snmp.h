#ifndef __snmp_h__
#define __snmp_h__
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
* $FileName: snmp.h$
* $Version : 3.6.13.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains definitions for using the SNMP
*   agent.
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/
#include <rtcs.h>

/*
** SNMP error status values
*/

#define SNMP_ERROR_noError              0
#define SNMP_ERROR_tooBig               1
#define SNMP_ERROR_noSuchName           2
#define SNMP_ERROR_badValue             3
#define SNMP_ERROR_readOnly             4
#define SNMP_ERROR_genErr               5

#define SNMP_ERROR_noAccess             6
#define SNMP_ERROR_wrongType            7
#define SNMP_ERROR_wrongLength          8
#define SNMP_ERROR_wrongEncoding        9
#define SNMP_ERROR_wrongValue          10
#define SNMP_ERROR_noCreation          11
#define SNMP_ERROR_inconsistentValue   12
#define SNMP_ERROR_resourceUnavailable 13
#define SNMP_ERROR_commitFailed        14
#define SNMP_ERROR_undoFailed          15
#define SNMP_ERROR_authorizationError  16
#define SNMP_ERROR_notWritable         17
#define SNMP_ERROR_inconsistentName    18

/* decide whether an error-status uses the error-index */
#define SNMP_ERROR_USEINDEX(n)         ((n) > 1)

/* not real error codes -- used internally */
#define SNMP_ERROR_PARSE               0x7F
#define SNMP_ERROR_noSuchObject        ASN1_TYPE_ERR_NOOBJECT
#define SNMP_ERROR_noSuchInstance      ASN1_TYPE_ERR_NOINST
#define SNMP_ERROR_endOfMibView        ASN1_TYPE_ERR_ENDOFMIB

/*
** MIB access types
*/
#define RTCSMIB_ACCESS_READ   0x01
#define RTCSMIB_ACCESS_WRITE  0x02

/*
** MIB node types
*/
#define RTCSMIB_NODETYPE_INT_CONST     1
#define RTCSMIB_NODETYPE_INT_PTR       2
#define RTCSMIB_NODETYPE_INT_FN        3
#define RTCSMIB_NODETYPE_UINT_CONST    4
#define RTCSMIB_NODETYPE_UINT_PTR      5
#define RTCSMIB_NODETYPE_UINT_FN       6
#define RTCSMIB_NODETYPE_DISPSTR_PTR   7
#define RTCSMIB_NODETYPE_DISPSTR_FN    8
#define RTCSMIB_NODETYPE_OCTSTR_FN     9
#define RTCSMIB_NODETYPE_OID_PTR       10
#define RTCSMIB_NODETYPE_OID_FN        11

#if SNMPCFG_HAVE_INT64
#define RTCSMIB_NODETYPE_INT64_CONST   12
#define RTCSMIB_NODETYPE_INT64_PTR     13
#define RTCSMIB_NODETYPE_INT64_FN      14
#define RTCSMIB_NODETYPE_UINT64_CONST  15
#define RTCSMIB_NODETYPE_UINT64_PTR    16
#define RTCSMIB_NODETYPE_UINT64_FN     17
#endif

/*
** Used internally by the MIB instance parser
*/
#define RTCSMIB_OP_GET              0
#define RTCSMIB_OP_GETNEXT          1
#define RTCSMIB_OP_GETFIRST         2
#define RTCSMIB_OP_FINDNEXT         3
#define RTCSMIB_OP_SET              4

#if RTCSCFG_ENABLE_SNMP_STATS
#define IF_SNMP_STATS_ENABLED(x) x
#else
#define IF_SNMP_STATS_ENABLED(x)
#endif


/***************************************
**
** Type definitions
**
*/

/*
** SNMP statistics for the SNMP MIB
*/

#if RTCSCFG_ENABLE_SNMP_STATS
typedef struct snmp_stats {
   RTCS_STATS_STRUCT COMMON;
   
   /* These are all included in ST_RX_DISCARDED */
   uint_32  ST_RX_BAD_PARSE;
   uint_32  ST_RX_BAD_VERSION;
   uint_32  ST_RX_BAD_COMMUNITY;
   uint_32  ST_RX_BAD_PDU;

   /* Breakdown of all packets received and sent */
   uint_32  ST_RX_VAR_GETS;
   uint_32  ST_RX_VAR_SETS;

   uint_32  ST_RX_GETREQ;
   uint_32  ST_RX_GETNEXTREQ;
   uint_32  ST_RX_RESPONSE;
   uint_32  ST_RX_SETREQ;
   uint_32  ST_RX_TRAP;
   uint_32  ST_RX_GETBULKREQ;
   uint_32  ST_RX_INFORM;
   uint_32  ST_RX_V2TRAP;
   uint_32  ST_RX_REPORT;
   uint_32  ST_RX_OTHER;

   uint_32  ST_TX_GETREQ;
   uint_32  ST_TX_GETNEXTREQ;
   uint_32  ST_TX_RESPONSE;
   uint_32  ST_TX_SETREQ;
   uint_32  ST_TX_TRAP;
   uint_32  ST_TX_GETBULKREQ;
   uint_32  ST_TX_INFORM;
   uint_32  ST_TX_V2TRAP;
   uint_32  ST_TX_REPORT;
   uint_32  ST_TX_OTHER;

   uint_32  ST_TX_TOOBIG;
   uint_32  ST_TX_NOSUCHNAME;
   uint_32  ST_TX_BADVALUE;
   uint_32  ST_TX_READONLY;
   uint_32  ST_TX_GENERR;
   uint_32  ST_TX_OTHERERR;

} SNMP_STATS, _PTR_ SNMP_STATS_PTR;
#endif
/*
** Internal state for the SNMP agent
*/

typedef struct snmp_parse {
#if RTCSCFG_ENABLE_SNMP_STATS
   SNMP_STATS  STATS;
#endif
   uint_32     trapsock;
   char_ptr    community[SNMPCFG_NUM_COMMUNITY];
   uint_32     communitylen[SNMPCFG_NUM_COMMUNITY];
   uint_32     currentcommunity;
   _ip_address trap_receiver_list[SNMPCFG_MAX_TRAP_RECEIVERS];

   uchar_ptr   inbuf;
   uint_32     inlen;
   uchar_ptr   outbuf;
   uint_32     outlen;

   uint_32     errstat;
   uint_32     errindex;
   uchar_ptr   errstatp;
   uchar_ptr   errindexp;

   uint_32     version;
   uint_32     pdutype;
   uint_32     nonrep;
   uint_32     reps;
} SNMP_PARSE, _PTR_ SNMP_PARSE_PTR;

/*
** Internal state for the MIB walker
*/

typedef struct rtcsmib_walk {
   TCPIP_PARM  COMMON;

   uint_8      pdutype;

   uchar_ptr   inbuf;
   uint_32     inlen;
   uchar_ptr   outbuf;
   uint_32     outlen;

   uint_32     errstat;

   uchar_ptr   oidp;
   uint_32     oidlen;
   uint_32     totlen;

} RTCSMIB_WALK, _PTR_ RTCSMIB_WALK_PTR;

/* Interface to pass messages to trap server */
typedef struct trap_parm {
   TCPIP_PARM           COMMON;
   _ip_address          address;
} TRAP_PARM, _PTR_ TRAP_PARM_PTR;

/*
** Structure of a single MIB node
*/

typedef struct rtcsmib_node {
   uint_32                    ID;

   struct rtcsmib_node _PTR_  NEXT;
   struct rtcsmib_node _PTR_  CHILD;
   struct rtcsmib_node _PTR_  PARENT;

   uint_32                    ACCESS;
   boolean (_CODE_PTR_        FIND)(uint_32, pointer, pointer _PTR_);
   boolean (_CODE_PTR_        PARSE)(RTCSMIB_WALK_PTR, uint_32, boolean (_CODE_PTR_)(uint_32, pointer, pointer _PTR_), boolean _PTR_, pointer _PTR_);
   uint_32                    ASN1_TYPE;

   struct rtcsmib_value _PTR_ GET;      

   uint_32 (_CODE_PTR_        SET)(pointer, uchar_ptr, uint_32);

} RTCSMIB_NODE, _PTR_ RTCSMIB_NODE_PTR;

typedef struct rtcsmib_value
{
    uint_32     TYPE; /* ignored in nonleaf nodes */
    pointer     PARAM;
} RTCSMIB_VALUE, _PTR_ RTCSMIB_VALUE_PTR ;

/* callback functions to provide MIB values dynamically */
typedef uint_32   (*RTCSMIB_UINT_FN_PTR)(pointer);
typedef int_32    (*RTCSMIB_INT_FN_PTR)(pointer);
typedef uchar_ptr (*RTCSMIB_DISPSTR_FN_PTR)(pointer);
typedef uchar_ptr (*RTCSMIB_OCTSTR_FN_PTR)(pointer, uint_32 _PTR_);
typedef RTCSMIB_NODE_PTR (*RTCSMIB_OID_FN_PTR)(pointer);    


/***************************************
**
** Prototypes
**
*/

/*
** Used by SNMP to make MIB requests:
*/

extern uint_32 RTCSMIB_request
(
   uint_8         pdutype,
   uchar_ptr      invarp,
   uint_32        invarlen,
   uchar_ptr      outvarp,
   uint_32        outvarlen,
   uint_32 _PTR_  writelen
);

/*
** Utilities for MIB implementations:
*/

extern void RTCSMIB_mib_add
(
   RTCSMIB_NODE_PTR  mib
);

extern int_32 RTCSMIB_int_read    (uchar_ptr, uint_32);
extern boolean RTCSMIB_id_read8   (RTCSMIB_WALK_PTR, uchar _PTR_);
extern boolean RTCSMIB_id_read32  (RTCSMIB_WALK_PTR, uint_32 _PTR_);
extern boolean RTCSMIB_id_write8  (RTCSMIB_WALK_PTR, uchar);
extern boolean RTCSMIB_id_write32 (RTCSMIB_WALK_PTR, uint_32);

extern boolean MIB_instance_zero
(
   RTCSMIB_WALK_PTR     mib,
   uint_32              op,
   boolean (_CODE_PTR_  find)(uint_32, pointer, pointer _PTR_),
   boolean _PTR_        found,
   pointer _PTR_        instance
);

extern void       RTCS_insert_trap_receiver_internal(TRAP_PARM _PTR_);
extern void       RTCS_remove_trap_receiver_internal(TRAP_PARM _PTR_);
extern _mqx_uint  RTCS_count_trap_receivers_internal(void);

extern void SNMP_trap_warmStart(void);
extern void SNMP_trap_coldStart(void);
extern void SNMP_trap_authenticationFailure(void);
extern void SNMP_trap_userSpec(RTCSMIB_NODE_PTR trap_node , uint_32 spec_trap, RTCSMIB_NODE_PTR enterprises );
extern void SNMPv2_trap_warmStart(void);
extern void SNMPv2_trap_coldStart(void);
extern void SNMPv2_trap_authenticationFailure(void);
extern void SNMPv2_trap_userSpec(RTCSMIB_NODE_PTR trap_node);
extern boolean SNMP_trap_select_community(char_ptr);
extern void SNMP_trap_linkDown(pointer);
extern void SNMP_trap_myLinkDown(pointer);
extern void SNMP_trap_linkUp(pointer);
extern void SNMPv2_trap_linkDown(pointer);
extern void SNMPv2_trap_linkUp(pointer);


#endif
/* EOF */
