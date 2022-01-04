#ifndef __ip6_h__
#define __ip6_h__
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
* $FileName: ip6.h$
* $Version : 3.8.1.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Definitions for the Internet Protocol 6.
*
*END************************************************************************/

#include "rtcspcb.h"
/******************************************************************
* Constants
*******************************************************************/
#define IP6_HEADSIZE            40      /* sizeof(IP6_HEADER)                 */

#define IP6_DEFAULT_MTU         1280    /* Minimum IPv6 datagram size which    */
                                        /* must be supported by all IPv6 hosts */
#define IP6_DEFAULT_HOPLIMIT    64      /* Default Hop Limit. */

/* IPIF_*() params.*/
typedef struct ip6_if_parm
{
   TCPIP_PARM           COMMON;    
   _rtcs_if_handle      ihandle;
   struct in6_addr      ip_addr;
   ip6_if_addr_type     ip_addr_type;
} 
IP6_IF_PARM, _PTR_ IP6_IF_PARM_PTR;

/******************************************************************
* Function Prototypes
*******************************************************************/
extern uint_32 IP6_init(void);
ICB_STRUCT_PTR IP6_open (uchar  protocol, IP_SERVICE_FN service, pointer private, uint_32_ptr status);
extern void IP6_service (RTCSPCB_PTR);
extern uint_32 IP6_send (RTCSPCB_PTR rtcs_pcb, uint_32 protocol, in6_addr *ipsrc, in6_addr *ipdest, _rtcs_if_handle ihandle_dest, uint_32 flags);
extern void IP6_IF_bind_addr  (IP6_IF_PARM_PTR);
extern void IP6_IF_unbind_addr (IP6_IF_PARM_PTR);

#endif

