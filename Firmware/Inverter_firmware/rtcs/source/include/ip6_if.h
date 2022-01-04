#ifndef __ip6_if_h__
#define __ip6_if_h__
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
* $FileName: ip6_if.h$
* $Version : 3.8.1.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Definitions for the Internet Protocol 6 Network Interfaces.
*
*END************************************************************************/

#include "rtcs.h"
#include "nd6.h"
#include "ipcfg.h"

/* Maxinmum number of IPv6 addresses per interface.*/ 
#define IP6_IF_ADDRESSES_MAX    (RTCSCFG_IP6_IF_ADDRESSES_MAX)

/* A lifetime value of all one bits (0xffffffff) represents infinity. */
#define IP6_IF_ADDRESS_LIFETIME_INFINITE    ND6_PREFIX_LIFETIME_INFINITE   

/*********************************************************************
 * Address information structure.
 *********************************************************************/
typedef struct ip6_if_addr_info
{
    in6_addr            ip_addr;                    /* IPv6 address.*/
    ip6_if_addr_state   ip_addr_state;              /* Address current state.*/
    ip6_if_addr_type    ip_addr_type;               /* How the address was acquired.*/
    in6_addr            solicited_multicast_addr;   /* Solicited-node multicast 
                                                     * group-address for assigned ip_addr.*/
    uint_32             creation_time;              /* Time of entry creation (in seconds).*/    
    uint_32             lifetime;                   /* Address lifetime (in seconds). 0xFFFFFFFF = Infinite Lifetime
                                                     * RFC4862. A link-local address has an infinite preferred and valid lifetime; it
                                                     * is never timed out.*/
    uint_32             prefix_length;              /* Prefix length (in bits). The number of leading bits
                                                     * in the Prefix that are valid. */                                        
    uint_32             dad_transmit_counter;       /* Counter used by DAD. Equals to the number 
                                                     * of NS transmits till DAD is finished.*/                                                    
    uint_32             state_time;                 /* Time of last state event.*/  
                                                    
} IP6_IF_ADDR_INFO, _PTR_ IP6_IF_ADDR_INFO_PTR;

struct ip_if; /* Just to avoid header conflicts.*/ //TBD 

/**********************************************************************
* Extension to the ip_if structure (in ip_prv.h).
***********************************************************************/
typedef struct ip6_if_struct
{
    struct ip_if            *next_if;                           /* Pointer to the next IF.*/
    struct ip_if            *prev_if;                           /* Pointer to the previous IF.*/
    uint_32                 scope_id;
    /* The IPv6 addressing architecture [1] allows multiple unicast
     * addresses to be assigned to interfaces. */
    IP6_IF_ADDR_INFO        address[IP6_IF_ADDRESSES_MAX];      /* RFC4862 5.2: A host maintains a list of addresses together with their
                                                                * corresponding lifetimes. The address list contains both
                                                                * autoconfigured addresses and those configured manually.
                                                                */
    ND6_CFG_PTR             ND6;                                /* Pointer to NDP control structure.*/ //TBD PFI=> NOT PRT
    void                    (*route)(RTCSPCB_PTR rtcs_pcb);     /* Routing ptotocol service function.*/
} IP6_IF_STRUCT, _PTR_ IP6_IF_STRUCT_PTR;

/************************************************************************
*     Function Prototypes
*************************************************************************/
extern boolean ip6_if_get_addr (_rtcs_if_handle ihandle, uint_32 n, IP6_IF_ADDR_INFO_PTR addr_info);
extern boolean ip6_if_is_my_addr(_rtcs_if_handle ihandle, in6_addr *ip_addr);
extern boolean ip6_if_is_my_solicited_multicast_addr(_rtcs_if_handle ihandle, in6_addr *ip_addr);

IP6_IF_ADDR_INFO_PTR ip6_if_get_addr_info(struct ip_if *if_ptr, in6_addr *ip_addr);
const in6_addr * ip6_if_select_src_addr(struct ip_if *if_ptr, in6_addr *dest_addr);
void ip6_if_unbind_addr ( struct ip_if *if_ptr, IP6_IF_ADDR_INFO_PTR if_addr );
uint_32 ip6_if_bind_addr (struct ip_if *if_ptr, in6_addr *addr, ip6_if_addr_type addr_type, uint_32 lifetime, uint_32 prefix_length);
uint_32 ip6_if_join (struct ip_if *if_ptr, const in6_addr *group_ip);
uint_32 ip6_if_leave(struct ip_if *if_ptr, const in6_addr *group_ip);
void ip6_if_add( struct ip_if *if_ptr );
void ip6_if_del( struct ip_if *if_ptr );
struct ip_if *ip6_if_get_by_n( uint_32 n );
struct ip_if *ip6_if_get_by_addr(in6_addr *addr);
struct ip_if *ip6_if_get_by_scope_id(uint_32 scope_id);
uint_32 ip6_if_get_scope_id(struct ip_if *if_ptr);
int ip6_if_addr_autoconf_set(struct ip_if *if_ptr, struct in6_addr *ipaddr);
void ip6_if_addr_timer(struct ip_if *if_ptr);
void ip6_if_route_instal(struct ip_if *if_ptr, void (*route)(RTCSPCB_PTR rtcs_pcb));  /* Instalation of Routing ptotocol service function.*/

/* For DEBUG needs only. */
void ip6_if_debug_print_addr_info( struct ip_if *if_ptr );

#endif /* __ip6_if_h__ */

