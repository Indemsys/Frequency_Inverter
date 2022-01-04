#ifndef __ip6_trace_h__
#define __ip6_trace_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
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
* $FileName: ip6_trace.h$
* $Version : 3.8.1.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the TCP/IP tracing feature.
*   It is based on FNET project.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"
#include "ethernet.h"
#include "icmp6_prv.h"

/******************************************************************
* Function Prototypes
*******************************************************************/
#if RTCSCFG_DEBUG_TRACE_ETH
    void trace_eth(char *str, ENET_HEADER_PTR eth_hdr);
#else
    #define trace_eth(str, eth_hdr)
#endif 

#if RTCSCFG_DEBUG_TRACE_IP6
    void trace_ip6(char *str, IP6_HEADER_PTR ip_hdr);
#else
    #define trace_ip6(str, ip_hdr)
#endif 

#if RTCSCFG_DEBUG_TRACE_ICMP6
    void trace_icmp6(char *str, ICMP6_HEADER_PTR icmp_hdr);
#else
    #define trace_icmp6(str, icmp_hdr)
#endif 


#endif /*__ip6_trace_h__*/

