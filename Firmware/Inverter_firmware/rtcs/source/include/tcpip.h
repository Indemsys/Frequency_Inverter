#ifndef __tcpip_h__
#define __tcpip_h__
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
* $FileName: tcpip.h$
* $Version : 3.8.10.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Definitions for the TCP/IP layer error codes.
*
*END************************************************************************/

#include "rtcstime.h"
#include "ipradix.h"
#include "checksum.h"
#include "ip.h"
#include "icmp.h"
#include "igmp.h"
#include "udp.h"
#include "tcp.h"
#include "icmp6.h"

/***************************************
**
** Type definitions
**
*/

typedef struct tcpip_message {
   MESSAGE_HEADER_STRUCT   HEAD;
   void (_CODE_PTR_        COMMAND)(pointer);
   pointer                 DATA;
} TCPIP_MESSAGE, _PTR_ TCPIP_MESSAGE_PTR;

/*  TCP Configuration
**    This information is persistent for the TCP layer.
*/
typedef struct tcpip_cfg_struct {

   uint_32     status;           /* task status - RTCS_OK => running */

} TCPIP_CFG_STRUCT, _PTR_ TCPIP_CFG_STRUCT_PTR;


/***************************************
**
** Function-specific type definitions
**
*/


/* IPIF_*() */
typedef struct ipif_parm {
   TCPIP_PARM           COMMON;
   pointer              mhandle;
   _rtcs_if_handle      ihandle;
   RTCS_IF_STRUCT_PTR   if_ptr;
   _ip_address          address;
   _ip_address          locmask;
   _ip_address          network;
   _ip_address          netmask;
   boolean              probe;
} IPIF_PARM, _PTR_ IPIF_PARM_PTR;


/* SOCK_selectall() and SOCK_select() */
typedef struct sockselect_parm {
   TCPIP_PARM                          COMMON;
   struct sockselect_parm _PTR_        NEXT;
   struct sockselect_parm _PTR_ _PTR_  PREV;
   TCPIP_EVENT                         EXPIRE;
   _rtcs_taskid                        owner;
   uint_32 _PTR_                       sock_ptr;
   uint_32 _PTR_                       writesock_ptr;
   uint_32 _PTR_                       exceptsock_ptr;
   uint_32                             sock_count;
   uint_32                             writesock_count;
   uint_32                             timeout;
   uint_32                             sock;
} SOCKSELECT_PARM, _PTR_ SOCKSELECT_PARM_PTR;


/***************************************
**
** Globals
**
*/

/* The TCP tick server */
extern uint_32 (_CODE_PTR_ TCP_tick)(void);


/***************************************
**
** Prototypes
**
*/

void TCPIP_task (pointer, pointer);

/* Select functions */
extern void    SOCK_selectall       (SOCKSELECT_PARM_PTR);
extern void    SOCK_select          (SOCKSELECT_PARM_PTR);

extern boolean SOCK_select_activity (SOCKET_STRUCT_PTR, int);
extern void    SOCK_select_block    (SOCKSELECT_PARM_PTR);
extern boolean SOCK_select_expire   (TCPIP_EVENT_PTR);
extern void    SOCK_select_unblock  (SOCKSELECT_PARM_PTR, uint_16);

#define SOCK_select_signal(a, b)    _SOCK_select_signal(a, b, 0)

extern void    _SOCK_select_signal   (uint_32, uint_16, uint_32);

/* BOOTP and DHCP common functions */

uint_32 BOOT_init
(
   void
);

uint_32 BOOT_open
(
   void (_CODE_PTR_)(RTCSPCB_PTR, UCB_STRUCT_PTR)
);

uint_32 BOOT_close
(
   void
);

uint_32 BOOT_send
(
   RTCSPCB_PTR       ,        /* [IN]     outgoing packet  */
   pointer                    /* [IN]     target interface */
);

void BOOT_service
(
   RTCSPCB_PTR       ,        /* [IN/OUT] incoming packet  */
   UCB_STRUCT_PTR             /* [IN]     target UCB       */
);


#endif
/* EOF */
