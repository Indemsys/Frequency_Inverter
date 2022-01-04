#ifndef __iptunnel_h__
#define __iptunnel_h__
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
* $FileName: iptunnel.h$
* $Version : 3.5.5.0$
* $Date    : Dec-8-2009$
*
* Comments:
*
*   This file contains the defines, externs and data
*   structure definitions required by IP over IP encapsulation.
*
*END************************************************************************/

#include "ip_prv.h"


/*********************************************************
**
** IPIP CONSTANTS
*/

#define IPIP_INITIAL_COUNT  4
#define IPIP_GROW_COUNT     4


/*********************************************************
**
** IPIP Structures
*/

/*
** IPIP_TUNNEL
**    Contains information about an IP over IP tunnel
*/
typedef struct ipip_tunnel {
   struct ipip_tunnel _PTR_ NEXT;       /* Points to next node in list */
   _ip_address    INNER_SOURCE;           /* Embedded header source */
   _ip_address    INNER_SOURCE_NETMASK;   /* Embedded header source mask */
   _ip_address    INNER_DEST;             /* Embedded header destination */
   _ip_address    INNER_DEST_NETMASK;     /* Embedded header dest mask */
   _ip_address    OUTER_SOURCE;           /* Source for outer header */
   _ip_address    OUTER_DEST;             /* Destination for outer header */
   uint_32        FLAGS;                  /* Behaviour flags */
} IPIP_TUNNEL, _PTR_ IPIP_TUNNEL_PTR;


/*
** IPIP_PARM
**    Used to pass information from the application to RTCS
*/
typedef struct  ipip_parm {
   TCPIP_PARM     COMMON;                 /* Common to all parm structs */
   IPIP_TUNNEL    TUNNEL;
} IPIP_PARM, _PTR_ IPIP_PARM_PTR;


/*
** IPIP_CFG_STRUCT
**    Contains the configuration information for the IP Tunnel device
*/
typedef struct ipip_cfg_struct {
   IPIP_TUNNEL_PTR   NEXT;             /* Linked list of tunnels */
   _rtcs_part        IPIP_PART;        /* Tunnel information part */
   IP_IF_PTR         IPIP_IF;          /* IPIP interface pointer */
   uint_32           TUNNELS;          /* Number of tunnels */
} IPIP_CFG_STRUCT,  _PTR_ IPIP_CFG_STRUCT_PTR;



#ifdef __cplusplus
extern "C" {
#endif

extern uint_32 IPIP_init (void);

extern void IPIP_service (RTCSPCB_PTR pcb_ptr, pointer dummy);

extern void IPIP_insert (IPIP_PARM_PTR);
extern void IPIP_delete (IPIP_PARM_PTR);

extern uint_32 IPIP_open  (IP_IF_PTR);
extern uint_32 IPIP_close (IP_IF_PTR);

extern uint_32 IPIP_send (IP_IF_PTR, RTCSPCB_PTR, _ip_address, _ip_address, pointer);

extern void IPIP_send_internal (RTCSPCB_PTR);

extern uint_32 IPIP_MTU (pointer);

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
