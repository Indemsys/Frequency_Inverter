#ifndef __arp_h__
#define __arp_h__
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
* $FileName: arp.h$
* $Version : 3.0.5.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   Address Resolution Protocol definitions.
*
*END************************************************************************/

#include "ip_prv.h"


/***************************************
**
** Prototypes
**
*/

extern uint_32 ARP_init
(
   void
);

extern uint_32 ARP_open
(
   IP_IF_PTR      if_ptr      /* [IN] IP interface structure */
);

extern uint_32 ARP_close
(
   IP_IF_PTR      if_ptr      /* [IN] IP interface structure */
);

extern void ARP_service
(
   RTCSPCB_PTR    rtcs_pcb    /* [IN] received packet */
);

extern uint_32 ARP_resolve
(
   IP_IF_PTR      if_ptr,     /* [IN] IP interface structure */
   RTCSPCB_PTR    rtcs_pcb,   /* [IN] packet to send */
   _ip_address    isrc,       /* [IN] source address */
   _ip_address    idest       /* [IN] destination address */
);

extern uint_32 ARP_request
(
   IP_IF_PTR      if_ptr,
   _ip_address    isrc,
   _ip_address    idest
);

extern boolean ARP_do_proxy
(
   IP_IF_PTR      iflocal,    /* [IN] the local interface */
   _ip_address    iplocal     /* [IN] the IP address to test */
);

extern boolean ARP_is_complete
(
  IP_IF_PTR      if_ptr,      /* [IN] IP interface structure */
  _ip_address    addr         /* [IN] the IP address to search for */
);

extern boolean  ARP_kill_entry
(
  IP_IF_PTR      if_ptr,       /* [IN] IP interface structure */
  _ip_address    addr          /* [IN] the IP address to remove from arp Q */
        
);

extern  RTCSPCB_PTR ARP_find_waiting
(
  IP_IF_PTR      if_ptr,       /* [IN] IP interface structure */
  _ip_address    addr          /* [IN] the IP address to remove from */
);


#endif
/* EOF */
