#ifndef __dhcp_h__
#define __dhcp_h__
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
* $FileName: dhcp.h$
* $Version : 3.0.5.0$
* $Date    : Feb-26-2009$
*
* Comments:
*
*   Dynamic Host Configuration Protocol definitions.
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/

#define DHCPOP_BOOTREQUEST    1
#define DHCPOP_BOOTREPLY      2

#define DHCPTYPE_DHCPDISCOVER 1
#define DHCPTYPE_DHCPOFFER    2
#define DHCPTYPE_DHCPREQUEST  3
#define DHCPTYPE_DHCPDECLINE  4
#define DHCPTYPE_DHCPACK      5
#define DHCPTYPE_DHCPNAK      6
#define DHCPTYPE_DHCPRELEASE  7
#define DHCPTYPE_DHCPINFORM   8

#define DHCPTYPE_ISVALID(type)   ((type) >= 1 && (type) <= 8)

#define DHCPFLAG_BROADCAST                0x8000

#define DHCP_MAGIC            0x63825363L
#define DHCP_MIN_MESSAGE_SIZE 576
/*
** DHCP options
*/
#define DHCPOPT_PAD           0
#define DHCPOPT_END           255
#define DHCPOPT_MASK          1
/* Start CR 1152 */
#define DHCPOPT_TIMEOFFSET    2
#define DHCPOPT_ROUTER        3
#define DHCPOPT_TIMESERVER    4
#define DHCPOPT_NAMESERVER    5
#define DHCPOPT_DNSSERVER     6
#define DHCPOPT_HOSTNAME      12
#define DHCPOPT_BOOTFILESIZE  13
#define DHCPOPT_DOMAINNAME    15
#define DHCPOPT_ADDRESS       50
#define DHCPOPT_LEASE         51
#define DHCPOPT_OVERLOAD      52
#define DHCPOPT_MSGTYPE       53
#define DHCPOPT_SERVERID      54
#define DHCPOPT_PARAMLIST     55
#define DHCPOPT_MSGSIZE       57
#define DHCPOPT_RENEWALL      58
#define DHCPOPT_REBINDING     59
#define DHCPOPT_CLASSID       60
#define DHCPOPT_CLIENTID      61
#define DHCPOPT_SERVERNAME    66
#define DHCPOPT_TFTPSERVER    66
#define DHCPOPT_FILENAME      67
#define DHCPOPT_BOOTFILENAME  67
/* End CR 1152 */
#define DHCPOPT_WWW_SRV       72
#define DHCPOPT_FINGER_SRV    73

#define DHCPSIZE_MAGIC        4
#define DHCPSIZE_MASK         4
#define DHCPSIZE_ADDRESS      4
#define DHCPSIZE_LEASE        4
#define DHCPSIZE_MSGTYPE      1
#define DHCPSIZE_SERVERID     4
#define DHCPSIZE_MSGSIZE      2

#define DHCP_MSGSIZE_MIN      576

#define DHCP_LEASE_INFINITE   0xFFFFFFFF
/*
**  Flags
*/
#define DHCP_SEND_INFORM_MESSAGE              0x00000001
#define DHCP_MAINTAIN_STATE_ON_INFINITE_LEASE 0x00000002
/* Start CR 1547 */
#define DHCP_SEND_PROBE                       0x00000004
/* End CR 1547 */

/*
** Initialization Structure for DHCP Client
*/

typedef struct  dhcp_data_struct {
   int_32      (_CODE_PTR_ CHOICE_FUNC)(uchar _PTR_, uint_32);
   void        (_CODE_PTR_ BIND_FUNC)  (uchar _PTR_, uint_32, _rtcs_if_handle);
   void        (_CODE_PTR_ REBIND_FUNC)(uchar _PTR_, uint_32, _rtcs_if_handle);
   boolean     (_CODE_PTR_ UNBIND_FUNC)(_rtcs_if_handle);
   boolean     (_CODE_PTR_ FAILURE_FUNC)(_rtcs_if_handle);
   uint_32     FLAGS;
/* Start CR 2199 */   
   boolean     (_CODE_PTR_ NAK_FUNC)  (uchar _PTR_, uint_32, _rtcs_if_handle);
/* End CR 2199 */   
} DHCP_DATA_STRUCT, _PTR_ DHCP_DATA_STRUCT_PTR;

/***************************************
** Contents of a DHCP Packet Header
*/

typedef struct dhcp_header
{
   uchar    OP[1];
   uchar    HTYPE[1];
   uchar    HLEN[1];
   uchar    HOPS[1];
   uchar    XID[4];
   uchar    SECS[2];
   uchar    FLAGS[2];
   uchar    CIADDR[4];
   uchar    YIADDR[4];
   uchar    SIADDR[4];
   uchar    GIADDR[4];
   uchar    CHADDR[16];
   uchar    SNAME[64];
   uchar    FILE[128];
} DHCP_HEADER, _PTR_ DHCP_HEADER_PTR;

/***************************************
**
** Function definitions
**
*/
extern uint_32 RTCS_if_bind_DHCP_flagged(_rtcs_if_handle, DHCP_DATA_STRUCT_PTR, uchar_ptr, uint_32);
extern uint_32 RTCS_if_bind_DHCP_timed(_rtcs_if_handle, DHCP_DATA_STRUCT_PTR, uchar_ptr, uint_32, uint_32);
extern uint_32 RTCS_if_bind_DHCP(_rtcs_if_handle, DHCP_DATA_STRUCT_PTR, uchar_ptr, uint_32);
extern uint_32 RTCS_if_rebind_DHCP(_rtcs_if_handle, _ip_address, _ip_address, uint_32, _ip_address, DHCP_DATA_STRUCT_PTR, uchar_ptr, uint_32);
extern uint_32 RTCS_request_DHCP_inform(_rtcs_if_handle, uchar_ptr, uint_32, _ip_address, _ip_address, void (_CODE_PTR_ inform_func)(uchar _PTR_, uint_32, _rtcs_if_handle));
extern void    DHCPCLNT_release     (_rtcs_if_handle);

extern uchar _PTR_ DHCPCLNT_find_option(uchar_ptr, uint_32, uchar);
extern uchar _PTR_ DHCP_find_option(uchar_ptr, uint_32, uchar);

extern boolean DHCP_option_int8     (uchar_ptr _PTR_, uint_32 _PTR_, uchar, uchar);
extern boolean DHCP_option_int16    (uchar_ptr _PTR_, uint_32 _PTR_, uchar, uint_16);
extern boolean DHCP_option_int32    (uchar_ptr _PTR_, uint_32 _PTR_, uchar, uint_32);
extern boolean DHCP_option_variable (uchar_ptr _PTR_, uint_32 _PTR_, uchar, uchar _PTR_, uint_32);
extern boolean DHCP_option_addr     (uchar_ptr _PTR_, uint_32 _PTR_, uchar, _ip_address);
extern boolean DHCP_option_addrlist (uchar_ptr _PTR_, uint_32 _PTR_, uchar, _ip_address _PTR_, uint_32);
extern boolean DHCP_option_string   (uchar_ptr _PTR_, uint_32 _PTR_, uchar, char_ptr);


#endif
/* EOF */
