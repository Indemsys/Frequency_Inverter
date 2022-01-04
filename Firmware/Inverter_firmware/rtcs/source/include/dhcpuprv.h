#ifndef __dhcpuprv_h__
#define __dhcpuprv_h__
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
* $FileName: dhcpuprv.h$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   Dynamic Host Configuration Protocol utility prototypes.
*
*END************************************************************************/

/***************************************
**
** Prototypes
**
*/
static void DHCPCLNT_fill_header ( DHCP_CLNT_STRUCT  _PTR_ );
static void DHCPCLNT_fill_options (DHCP_CLNT_STRUCT  _PTR_,
                                   TCPIP_PARM_IF_DHCP _PTR_ );
static void DHCPCLNT_open ( DHCP_CLNT_STRUCT _PTR_ );
static void DHCPCLNT_modify_options ( DHCP_CLNT_STRUCT _PTR_ );
static void DHCPCLNT_send_one_shot ( DHCP_CLNT_STRUCT _PTR_ );
static boolean DHCPCLNT_rebind_lease ( TCPIP_EVENT _PTR_ );
static boolean DHCPCLNT_renew_lease ( TCPIP_EVENT _PTR_ );
static boolean DHCPCLNT_terminate_lease ( TCPIP_EVENT _PTR_ );
static boolean DHCPCLNT_bind_attempt_timeout( TCPIP_EVENT _PTR_ );
static void DHCPCLNT_decline ( DHCP_CLNT_STRUCT _PTR_, uint_32 );
static void DHCPCLNT_parse_offer ( DHCP_CLNT_STRUCT _PTR_, RTCSPCB _PTR_ );
static void DHCPCLNT_copy_binding ( DHCP_CLNT_STRUCT _PTR_ );
static boolean DHCPCLNT_verify_address ( DHCP_CLNT_STRUCT _PTR_ );
static uchar DHCPCLNT_verify_packet (DHCP_CLNT_STRUCT _PTR_, RTCSPCB_PTR, uchar _PTR_, uint_32 _PTR_, _ip_address _PTR_, _ip_address _PTR_, uint_32 _PTR_);
static void DHCPCLNT_service_offer (DHCP_CLNT_STRUCT _PTR_, RTCSPCB_PTR, uchar _PTR_, uint_32, _ip_address, _ip_address);
static void DHCPCLNT_service_ack (DHCP_CLNT_STRUCT _PTR_, RTCSPCB_PTR, uchar _PTR_, uint_32, IP_IF_PTR);
/* Start CR 2199 */
static void DHCPCLNT_service_nak (DHCP_CLNT_STRUCT _PTR_, uchar _PTR_, uint_32);
/* End CR 2199 */  
static void DHCPCLNT_set_timed_events(DHCP_CLNT_STRUCT _PTR_);

#endif
/* EOF */
