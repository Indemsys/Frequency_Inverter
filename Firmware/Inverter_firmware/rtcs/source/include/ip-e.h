#ifndef __ipe_h__
#define __ipe_h__
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
* $FileName: ip-e.h$
* $Version : 3.8.6.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the private defines, externs and
*   data structure definitions required by the IP-E layer.
*
*END************************************************************************/

#include "ip_prv.h"


/***************************************
**
** Prototypes
**
*/

extern uint_32 IPE_open  (IP_IF_PTR);
extern uint_32 IPE_close (IP_IF_PTR);
extern uint_32 IPE_send  (IP_IF_PTR, RTCSPCB_PTR, uint_16, uchar[6] );
extern uint_32 IPE_send_IP (IP_IF_PTR, RTCSPCB_PTR, _ip_address, _ip_address, pointer);
extern uint_32 IPE_send_IP_ucast  (IP_IF_PTR, RTCSPCB_PTR, uchar[6]);
extern uint_32 IPE_send_ARP_bcast (IP_IF_PTR, RTCSPCB_PTR);
extern uint_32 IPE_send_ARP_ucast (IP_IF_PTR, RTCSPCB_PTR, uchar[6]);

extern void IPE_recv (PCB_PTR, pointer, void (_CODE_PTR_  service)(RTCSPCB_PTR));
extern void IPE_recv_IP  (PCB_PTR, pointer);
extern void IPE_recv_ARP (PCB_PTR, pointer);

extern uint_32 IPE_join  (IP_IF_PTR, _ip_address);
extern uint_32 IPE_leave (IP_IF_PTR, _ip_address);

#if RTCSCFG_ENABLE_IP6
extern uint_32 IP6E_open (IP_IF_PTR if_ptr);    /* Called by IPE_open.*/
extern uint_32 IP6E_close (IP_IF_PTR if_ptr);   /* Called by IPE_open.*/
extern void IP6E_recv_IP  (PCB_PTR, pointer);
extern uint_32 IP6E_send( IP_IF_PTR, RTCSPCB_PTR, in6_addr *, in6_addr *);
extern uint_32 IP6E_join  (IP_IF_PTR, const in6_addr *);
extern uint_32 IP6E_leave (IP_IF_PTR, const in6_addr *);
#endif /* RTCSCFG_ENABLE_IP6 */

#endif
/* EOF */
