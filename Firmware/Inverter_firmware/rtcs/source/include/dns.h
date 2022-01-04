#ifndef __dns_h__
#define __dns_h__
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
* $FileName: dns.h$
* $Version : 3.8.11.0$
* $Date    : Aug-10-2011$
*
* Comments:
*
*   Definitions for use by either the Domain Name Server
*   or Resolver.
*
*END************************************************************************/

extern void DNS_Resolver_task      (pointer, pointer);
extern void DNS_Name_Server_task   (uint_32);
extern void DNS_Stub_Resolver_task (uint_32);

/***************************************
**
** Constants
**
*/

#define DNSALLOC_SIZE                       4


#define DNS_NAME_SERVER_MODULE              1
#define DNS_FULL_RESOLVER_MODULE            2
#define DNS_STUB_RESOLVER_MODULE            3

/* maximum UDP size is 512 bytes, not counting headers */
#define DNS_MAX_PACKET_SIZE                 1024
#define DNS_MAX_UDP_MESSAGE_SIZE            512
#define DNS_MAX_IP_STRING_SIZE              16

/* maximum number of query tries for one DNS server address */
#define DNS_MAX_NUMBER_OF_QUERY_TRIES       5

/* picked 400 for now, need to find exact size that won't overflow a UDP send
   with headers, ect... SMR June 19, 1998 */
#define DNS_NULL_RECORD_SIZE                400

#define DNS_MAX_LABEL_LENGTH_ARRAY_SIZE     130
#define DNS_LAST_LABEL_ALLOWED              127
#define DNS_NAME_CONTROL_BUFFER             DNS_LAST_LABEL_ALLOWED + 1
#define DNS_NAME_LENGTH_BUFFER              DNS_MAX_LABEL_LENGTH_ARRAY_SIZE - 1
#define DNS_MAX_LABEL_LENGTH                63
#define DNS_MAX_CHARS_IN_NAME               0xFF
#define DNS_MAX_ID_CHECK                    0xFFFE
#define DNS_NAME_ADDITION_ERROR             0x00BADADD
#define DNS_MAX_ALLOWED_NAMES               30
/* one week, as required by RFC 1035 */
#define DNS_MAX_ALLOWED_TTL                 604800L
/* ten minutes as required by RFC 2308 */
#define DNS_NAME_ERROR_TTL                  600L
/*
** 4 seconds is BIND standard, but any reasonable value will do.
** This value is kept in seconds so that TTL for information
** coming in from the net can be kept in a consistant format and
** all functions can be kept common for all TTL's
*/
#define DNS_QUERY_RETRY_TIME                4L /* in seconds */
#define DNS_SERVER_RETRY_TIME               4000
#define DNS_RESOLVER_RETRY_TIME             4000
/* timeout for the response from the DNS server */
#define DNS_QUERY_TIMEOUT                   40000 

/* Name server and resolver ports and IP addresses */

#define DNS_NAMESERVER_PORT                 53
/* Start CR 2176 */
#define DNS_RESOLVER_PORT                   1024
/* End CR 2176 */

#define DNS_RESOLVER_IP_ADDR                INADDR_LOOPBACK


/* these are used to check and add to cached data */

#define DNS_NS_AUTH_DATA              1
#define DNS_NS_CACHE_DATA             2
#define DNS_RES_CACHE_DATA            3

/* RFC 1035 suggests 8 as a maximum, but it is a SHOULD item, not a MUST. */
#define DNS_MAX_NUM_CNAME_RETRIES     8

/* This is used to assign the size of a global buffer. */
#define DNS_MAX_NAMES  MAX_HOST_ALIASES
#define DNS_MAX_ADDRS  MAX_HOST_ADDRESSES


/* Error codes specific to DNS - backward compatibility */
#define DNS_QUERY_OK                        RTCSERR_DNS_QUERY_OK
#define DNS_NO_NAME_SERVER_RESPONSE         RTCSERR_DNS_NO_NAME_SERVER_RESPONSE
#define DNS_UNABLE_TO_OPEN_SOCKET           RTCSERR_DNS_UNABLE_TO_OPEN_SOCKET
#define DNS_UNABLE_TO_BIND_SOCKET           RTCSERR_DNS_UNABLE_TO_BIND_SOCKET
#define DNS_UNABLE_TO_UPDATE_ROOT_SERVERS   RTCSERR_DNS_UNABLE_TO_UPDATE_ROOT_SERVERS
#define DNS_SOCKET_RECV_ERROR               RTCSERR_DNS_SOCKET_RECV_ERROR
#define DNS_UNABLE_TO_SEND_QUERY            RTCSERR_DNS_UNABLE_TO_SEND_QUERY
#define DNS_NO_RESPONSE_FROM_RESOLVER       RTCSERR_DNS_NO_RESPONSE_FROM_RESOLVER
#define DNS_PACKET_RECEPTION_ERROR          RTCSERR_DNS_PACKET_RECEPTION_ERROR
#define DNS_INVALID_NAME                    RTCSERR_DNS_INVALID_NAME
#define DNS_INVALID_IP_ADDR                 RTCSERR_DNS_INVALID_IP_ADDR
#define DNS_ALL_SERVERS_QUERIED             RTCSERR_DNS_ALL_SERVERS_QUERIED
#define DNS_INVALID_LOCAL_NAME              RTCSERR_DNS_INVALID_LOCAL_NAME
#define DNS_UNABLE_TO_ALLOCATE_MEMORY       RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY
#define DNS_UNABLE_TO_FREE_SOCKET           RTCSERR_DNS_UNABLE_TO_FREE_SOCKET
#define DNS_VALID_IP_ADDR                   RTCSERR_DNS_VALID_IP_ADDR
#define DNS_INITIALIZATION_FAILURE          RTCSERR_DNS_INITIALIZATION_FAILURE
#define DNS_NOT_IMPLEMENTED                 RTCSERR_DNS_NOT_IMPLEMENTED
#define DNS_UNABLE_TO_CREATE_PARTITION      RTCSERR_DNS_UNABLE_TO_CREATE_PARTITION
#define DNS_SHORT_MESSAGE_ERROR             RTCSERR_DNS_SHORT_MESSAGE_ERROR


/* Any value from 64 to 255 will do if there is a conflict with these */
#define DNS_APPEND_LOCAL_NAME               0xFF
#define DNS_NOT_DOTTED_NAME_FORMAT          0xFE


/* Query Codes */

#define DNS_STANDARD_QUERY                  0x0000
#define DNS_INVERSE_QUERY                   0x0800
#define DNS_STATUS_REQUEST                  0x1000
#define DNS_RECURSION_DESIRED               0x0100

/* QDCOUNT Codes */
#define DNS_SINGLE_QUERY                    0x0001
#define DNS_DOUBLE_QUERY                    0x0002
#define DNS_TRIPLE_QUERY                    0x0003

/* ANCOUNT Codes */
#define DNS_NO_ANSWER                       0x0000
#define DNS_SINGLE_ANSWER                   0x0001
#define DNS_DOUBLE_ANSWER                   0x0002
#define DNS_TRIPLE_ANSWER                   0x0003
#define DNS_QADRUPLE_ANSWER                 0x0004

/* Response Codes  */

#define DNS_RESPONSE                        0x8000
#define DNS_RECURSION_DESIRED               0x0100
#define DNS_RECURSION_AVAILABLE             0x0080
#define DNS_AUTHORITATIVE_ANSWER            0x0400
#define DNS_TRUNCATION                      0x0200
#define DNS_NO_ERROR                        0x0000
#define DNS_FORMAT_ERROR                    0x0001
#define DNS_SERVER_FAILURE                  0x0002
#define DNS_NAME_ERROR                      0x0003
#define DNS_AUTHORITATIVE_NAME_ERROR        0x0403
#define DNS_QUERY_NOT_IMPLEMENTED           0x0004
#define DNS_QUERY_REFUSED                   0x0005

#define DNS_RCODE_MASK                      0x000F
#define DNS_OPCODE_MASK                     0x7800
#define DNS_COMPRESSED_NAME_MASK            0xC0
#define DNS_COMPRESSED_LOCATION_MASK        0x3FFF

/* These are needed for the TI board */
#define DNS_UINT_16_MASK                    0xFFFF
#define DNS_CHAR_MASK                       0xFF

/* TYPE and QTYPE definitions [RFC 1035] -- all TYPES are valid QTYPES
   Those marked EXPERIMENTAL or OBSOLETE should never be sent,
   however provisions should be made in case they are received  */


/* Class specific TYPES */

#define DNS_A          0x0001   /* A host address */
#define DNS_MD         0x0003   /* Mail Destination (OBSOLETE) */
#define DNS_MF         0x0004   /* Mail Forward     (OBSOLETE) */
#define DNS_MB         0x0007   /* Mailbox Domain Name (EXPERIMENTAL) */
#define DNS_MG         0x0008   /* Mail Group Member (EXPERIMENTAL) */
#define DNS_MR         0x0009   /* Mail Rename Domain Name (EXPERIMENTAL) */
#define DNS_NULL       0x000A   /* Null Resource Record */
#define DNS_WKS        0x000B   /* Service Description */
#define DNS_HINFO      0x000D   /* Host information */
#define DNS_MINFO      0x000E   /* Mailbox or mail list information */
#define DNS_MX         0x000F   /* Mail Exchange */
#define DNS_TXT        0x0010   /* Text Strings */

/* Types to be used by all classes */
#define DNS_NS         0x0002   /* Authoritative name server */
#define DNS_SOA        0x0006   /* Start of Zone of Authority */
#define DNS_CNAME      0x0005   /* Canonical Name for an alias */
#define DNS_PTR        0x000C   /* Domain name pointer */

/* QTYPES */

#define DNS_AXFR       0x00FC   /* Request for transfer of entire zone */
#define DNS_MAILB      0x00FD   /* Request for mailbox related records */
#define DNS_MAILA      0x00FE   /* Request for mail agent RR's (OBSOLETE) */

/* This is used for both QTYPE and QCLASS definitions */
#define DNS_ALL        0x00FF   /* Request for all records or for all classes */


/* CLASS values */

#define DNS_IN         0x0001   /* The Internet */
#define DNS_CS         0x0002   /* CSNET Class   (OBSOLETE) */
#define DNS_CH         0x0003   /* CHAOS Class  */
#define DNS_HS         0x0004   /* Hessoid [Dyer 87] */


#define DNS_RESERVED_CLASS 0xFFFF

 /***************************************
**
** Structure Definitions
**
*/


/*
**  Resource Record Header Structure
*/

typedef struct dns_rr_header {
   uint_32                       LAST_TTL_CHECK;
   struct dns_rr_header   _PTR_  NEXT;
   uchar _PTR_                   DOMAIN_NAME_PTR;
   int_32                        TTL;
   uint_16                       CLASS;
   uint_16                       TYPE;

}DNS_RR_HEADER_STRUCT, _PTR_ DNS_RR_HEADER_STRUCT_PTR;


/*
** Record-specific data structures, traditional record types.
*/

typedef struct dns_addr_record_struct {
   DNS_RR_HEADER_STRUCT             RR;
   _ip_address                      IPADDR;

}DNS_ADDR_RECORD_STRUCT, _PTR_ DNS_ADDR_RECORD_STRUCT_PTR;

typedef struct dns_name_record_struct {
   DNS_RR_HEADER_STRUCT             RR;
   uchar _PTR_                      ANS_NAME_PTR;

}DNS_NAME_RECORD_STRUCT, _PTR_ DNS_NAME_RECORD_STRUCT_PTR;


typedef struct dns_null_record_struct {
   DNS_RR_HEADER_STRUCT             RR;
   uchar                            NULL_BUFFER[DNS_NULL_RECORD_SIZE];

}DNS_NULL_RECORD_STRUCT, _PTR_ DNS_NULL_RECORD_STRUCT_PTR;


/*
** Structure used to maintain the linked list of resource records.
** NAME_SIZE is used for parsing name into response buffers of
** correct size.
*/

typedef struct dns_rr_list_struct {

   uchar                       _PTR_  NAME_PTR;
   DNS_RR_HEADER_STRUCT        _PTR_  RR_HEAD_PTR;
   struct dns_rr_list_struct   _PTR_  NEXT;

}DNS_RR_LIST_STRUCT, _PTR_ DNS_RR_LIST_STRUCT_PTR;


/*
** Domain Name Server Response Resource Record Structure
** This is defined to make writing a response easier.  This section of
** the response buffer should have a name first, this information second,
** and then response information of RDLENGTH after the structure.
*/

typedef struct dns_response_rr_middle_struct {

   uchar         TYPE[2];
   uchar         CLASS[2];
   uchar         TTL[4];
   uchar         RDLENGTH[2];

}DNS_RESPONSE_RR_MIDDLE_STRUCT, _PTR_ DNS_RESPONSE_RR_MIDDLE_STRUCT_PTR;

/*
** Domain Name Server Message Header Structure
*/

typedef struct dns_message_header_struct {

  uchar          ID[2];          /* Identifier used to match replies to
                                    outstanding queries */
  uchar          CONTROL[2];     /* Bits set to define query or response */
  uchar          QDCOUNT[2];     /* Number of entries in question section */
  uchar          ANCOUNT[2];     /* Number of RR's in answer section */
  uchar          NSCOUNT[2];     /* Number of Name Server RR's in the
                                    authority records   */
  uchar          ARCOUNT[2];     /* Number of RR's in the additional records */

}DNS_MESSAGE_HEADER_STRUCT, _PTR_ DNS_MESSAGE_HEADER_STRUCT_PTR;

/*
** Domain Name Server Message Tail Structure
*/

typedef struct dns_message_tail_struct {

  uchar       QTYPE[2];
  uchar       QCLASS[2];

}DNS_MESSAGE_TAIL_STRUCT, _PTR_ DNS_MESSAGE_TAIL_STRUCT_PTR;


/*
** Domain Name Server Server List Structure.  This structure is used
** exclusively by the resolver to keep a list of Name Servers to connect
** to when a query is made. Used only in the Root Server Structure
*/

typedef struct dns_slist_struct {

   char            _PTR_  NAME_PTR;
   struct dns_slist_struct _PTR_ NEXT;
   _ip_address            IPADDR;
   uint_32                LAST_TTL_CHECK;
   int_32                 TTL;
   uint_32                NUMBER_OF_HITS;
   uint_32                NUMBER_OF_TRIES;
   uint_32                AVG_RESPONSE_TIME;
   uint_16                TYPE;
   uint_16                CLASS;

}DNS_SLIST_STRUCT, _PTR_ DNS_SLIST_STRUCT_PTR;

/*
** Canonical Name Structure
*/

typedef struct dns_cname_struct {
   uchar                     _PTR_ CNAME_PTR;
   struct dns_cname_struct _PTR_ NEXT;
} DNS_CNAME_STRUCT, _PTR_ DNS_CNAME_STRUCT_PTR;

/*
**  Domain Name Server Query Id Structure. This structure is used in a
**  linked list of all outstanding queries.
**  The entries are deleted when expired or answered.
*/

typedef struct dns_internal_query_struct {


   _ip_address                     SERVER_IPADDR;
   uint_16                         QUERY_ID;
   uint_16                         CONTROL;
   uint_16                         QTYPE;
   uint_16                         QCLASS;
   /*
   ** This field is to keep track of which query was the reason for an
   ** internal query.
   */
   uint_16                         ORIGINAL_QID;
   uint_16                         RESERVED;
   int_32                          TTL;
   uint_32                         QUERY_RETRIES;
   uint_32                         LAST_TTL_CHECK;
   uchar                     _PTR_ QNAME_PTR;
   DNS_CNAME_STRUCT          _PTR_ CNAME;
   struct dns_internal_query_struct _PTR_ NEXT;
   sockaddr_in                     CLIENT_SIN;
   boolean                         INTERNAL_QUERY;

} DNS_INTERNAL_QUERY_STRUCT, _PTR_ DNS_INTERNAL_QUERY_STRUCT_PTR;

/*
** DNS Configuration Structure.  Used so DNS server and resolver functions
** only need to have one parameter passed to them and keep all necessary
** structures private.
*/

typedef struct dns_control_struct {

   DNS_INTERNAL_QUERY_STRUCT _PTR_ NS_QUERY_HEAD_PTR;
   DNS_INTERNAL_QUERY_STRUCT _PTR_ RES_QUERY_HEAD_PTR;
   DNS_RR_LIST_STRUCT        _PTR_ NS_ZONE_AUTH_LIST;
   DNS_RR_LIST_STRUCT        _PTR_ CACHE_PTR;
   DNS_SLIST_STRUCT          _PTR_ ROOT_SLIST_PTR;
   DNS_SLIST_STRUCT          _PTR_ LOCAL_SLIST_PTR;
   uchar                     _PTR_ NS_BUFFER;
   uchar                     _PTR_ RES_BUFFER;
   int_32                          NS_BUFFER_SIZE;
   int_32                          RES_BUFFER_SIZE;
   uint_32                         NS_SOCKET;
   uint_32                         RES_SOCKET;
   boolean                         UPDATING;
   _rtcs_mutex                     CACHE_MUTEX;
   uint_32                         NUM_NAMES_CACHED;
   _rtcs_part                      INT_QUERY_PARTID;
   _rtcs_part                      ADDR_RR_PARTID;
   _rtcs_part                      NAME_RR_PARTID;
   _rtcs_part                      RR_LIST_PARTID;
   _rtcs_part                      CNAME_PARTID;
   _rtcs_mutex                     SLIST_MUTEX;
} DNS_CONTROL_STRUCT, _PTR_ DNS_CONTROL_STRUCT_PTR;

/* Used to keep from blowing the stack in a recursive call */
typedef struct dns_recursive_cache_struct {
   DNS_INTERNAL_QUERY_STRUCT _PTR_ NEW_QUERY_PTR;
   DNS_CONTROL_STRUCT        _PTR_ CONTROL_PTR;
   DNS_RR_LIST_STRUCT        _PTR_ CACHE_HEAD;
   uchar                     _PTR_ NAME_PTR;
   uchar                     _PTR_ BUFFER_PTR;
   int_32                    _PTR_ BUFFER_SIZE_PTR;
   uint_32                         NUM_TRIES;
} DNS_RECURSIVE_CACHE_STRUCT, _PTR_ DNS_RECURSIVE_CACHE_STRUCT_PTR;


/* Function definitions required by DNS_gethostbyname and DNS_gethostbyaddr */

DNS_RR_LIST_STRUCT _PTR_ DNS_dynamically_allocate_zone_list
(
     DNS_CONTROL_STRUCT _PTR_ control_ptr
);

uint_32 DNS_is_dotted_domain_name
(
     uchar _PTR_ domain_name,
     uchar _PTR_ qname_buffer
);

uint_32 DNS_is_dotted_IP_addr
(
     uchar _PTR_ addr_ptr,
     uchar _PTR_ qname_buffer
);

HOSTENT_STRUCT _PTR_ DNS_parse_UDP_response
(
     uchar _PTR_ buffer_ptr,
     uchar _PTR_ name_ptr,
     uint_16     query_type
);

HOSTENT_STRUCT _PTR_ DNS_query_resolver_task
(
     uchar _PTR_ name,
     uint_16    query_type
);

HOSTENT_STRUCT_PTR DNS_check_local_host_file
(
     uchar _PTR_ name
);

uint_32 DNS_is_valid_domain_name
(
     uchar    _PTR_         buffer_ptr,
     uchar    _PTR_         domain_name,
     uchar    _PTR_  _PTR_  local_name_buffer
);


/*
* extern struct of initial root server information
*/

extern DNS_SLIST_STRUCT   DNS_A_Root_server[];
extern char         DNS_Local_network_name[];
extern uint_32 DNS_CACHE_NAME_LIMIT;
extern DNS_RR_LIST_STRUCT DNS_Zone_of_Authority_list[];

/* Start CR 2257 */
extern DNS_SLIST_STRUCT _PTR_ DNS_First_Local_server;
extern char         DNS_Local_server_name[];
extern void DNS_init_slist_entry   (DNS_SLIST_STRUCT _PTR_);
extern uint_32 DNS_append_slist_entry (DNS_SLIST_STRUCT _PTR_);
extern uint_32 DNS_prepend_slist_entry (DNS_SLIST_STRUCT _PTR_);
extern uint_32 DNS_remove_slist_entry (_ip_address);
extern DNS_SLIST_STRUCT _PTR_ DNS_find_slist_entry (_ip_address);
#define DNS_add_slist_entry(x) DNS_prepend_slist_entry(x)
/* End CR 2257 */

#endif

/* EOF */
