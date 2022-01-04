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
* $FileName: ip6-e.c$
* $Version : 3.8.1.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the interface between IPv6 and
*   the Ethernet packet driver.
*
*END************************************************************************/

#include <string.h>
#include <rtcs.h>

#if RTCSCFG_ENABLE_IP6

#include <ethernet.h>
#include <enet.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "arp.h"
#include "ip-e.h"
#include "ip6_trace.h"

/************************************************************************
 * Ethernet Multicast Address, for IPv6.
 ***********************************************************************/
#define IP6E_ADDR_MC(eth_addr,ip6)              \
            (eth_addr)[0] = 0x33,               \
            (eth_addr)[1] = 0x33,               \
            (eth_addr)[2] = ip6->s6_addr[12],   \
            (eth_addr)[3] = ip6->s6_addr[13],   \
            (eth_addr)[4] = ip6->s6_addr[14],   \
            (eth_addr)[5] = ip6->s6_addr[15]

/************************************************************************
* NAME: IP6E_open
*
* DESCRIPTION: Registers IPv6 with an Ethernet packet driver, 
*              Generates & Assign Link-local Address.
*************************************************************************/
uint_32 IP6E_open (IP_IF_PTR if_ptr )
{
    uint_32  error;

    /* Register IP6.
     */
    error = ENET_open(if_ptr->HANDLE, ENETPROT_IP6, IP6E_recv_IP, if_ptr);
    if (error) 
    {
        goto OPEN_FAILED_1;
    } 
    
  
    /* Init Neighbor Discovery.*/
    error = nd6_open(if_ptr);
    if (error) 
    {
        goto OPEN_FAILED_2;
    }
    
    /* Add to IF list.*/ //TBD make it common for all interfaces.
    ip6_if_add(if_ptr);

    /* RFC4861 6.3.3: The host joins the all-nodes multicast address on all 
     * multicastcapable interfaces.
     */
    ip6_if_join (if_ptr, &in6addr_linklocal_allnodes);

    /* To speed the autoconfiguration process, a host may generate its linklocal
     * address (and verify its uniqueness) in parallel with waiting
     * for a Router Advertisement. Because a router may delay responding to
     * a Router Solicitation for a few seconds, the total time needed to
     * complete autoconfiguration can be significantly longer if the two
     * steps are done serially.
     */


    /* Link-Local Address Generation/Auto configuration.
     * It comprises of '1111111010' as the first ten bits followed by 54 zeroes 
     * and a 64 bit interface identifier.
     * For all autoconfiguration types, a link-local address is always configured. 
     */
    ip6_if_bind_addr (if_ptr, (in6_addr *)&in6addr_any, IP6_ADDR_TYPE_AUTOCONFIGURABLE, IP6_IF_ADDRESS_LIFETIME_INFINITE, ND6_PREFIX_LENGTH_DEFAULT);     

    /* RFC4862: The next phase of autoconfiguration involves obtaining a Router
     * Advertisement or determining that no routers are present.  If routers
     * are present, they will send Router Advertisements that specify what
     * sort of autoconfiguration a host can do.
     * To obtain an advertisement quickly, a host sends one or more Router
     * Solicitations to the all-routers multicast group.
     */ 
    nd6_rd_start(if_ptr);
    
    return RTCS_OK;
    
    /* Error handler.
     */
OPEN_FAILED_2:
    ENET_close(if_ptr->HANDLE, ENETPROT_IP6);  
OPEN_FAILED_1:
    return error;
}

/************************************************************************
* NAME: IP6E_close
*
* DESCRIPTION: Deregisters IPv6 from an Ethernet packet driver, 
*              etc.
*************************************************************************/
uint_32 IP6E_close (IP_IF_PTR if_ptr )
{
    uint_32 error;
    int     i;

    /* Delete from IF list.*/ //TBD make it common for all interfaces.
    ip6_if_del(if_ptr);
    
    /* Relaese ND for this interface.
     */
    nd6_close(if_ptr);    
    
    /* Unbind all addresses. 
     */
    for(i = 0; i < IP6_IF_ADDRESSES_MAX; i++)
    {
        ip6_if_unbind_addr (if_ptr, &if_ptr->IP6_IF.address[i]); 
    }
    
    /* Leaves the all-nodes multicast address.
     */
    ip6_if_leave (if_ptr, &in6addr_linklocal_allnodes);
      
    /* Deregister IP6.
     */
    error = ENET_close(if_ptr->HANDLE, ENETPROT_IP6);
     
    return error;
}

/************************************************************************
* NAME: IP6E_recv_IP
* RETURNS : RTCS_OK or error code
* DESCRIPTION: Services an IPv6 packet.
*************************************************************************/
void IP6E_recv_IP ( PCB_PTR  pcb    /* [IN] the received packet */,
                    pointer  handle /* [IN] the IP interface structure */)
{ 
   RTCSLOG_FNE3(RTCSLOG_FN_IP6E_recv_IP, pcb, handle);
   IPE_recv(pcb, handle, IP6_service);
   RTCSLOG_FNX1(RTCSLOG_FN_IP6E_recv_IP);
}

/************************************************************************
* NAME: IP6E_join
* RETURNS : RTCS_OK or error code
* DESCRIPTION: Joins an IPv6 multicast group..
*************************************************************************/
uint_32 IP6E_join ( IP_IF_PTR if_ptr        /* [IN] the IP interface structure */,
                    const in6_addr *group   /* [IN] the multicast group */)
{
   uchar    enetgroup[6];

   IP6E_ADDR_MC(enetgroup, group);
   return ENET_join(if_ptr->HANDLE, ENETPROT_IP6, enetgroup);
}

/************************************************************************
* NAME: IP6E_leave
* RETURNS : RTCS_OK or error code
* DESCRIPTION: Leaves an IPv6 multicast group.
*************************************************************************/
uint_32 IP6E_leave( IP_IF_PTR if_ptr        /* [IN] the IP interface structure */, 
                    const in6_addr *group   /* [IN] the multicast group */)
{
   uchar    enetgroup[6];

   IP6E_ADDR_MC(enetgroup, group);
   return ENET_leave(if_ptr->HANDLE, ENETPROT_IP6, enetgroup);
}

/************************************************************************
* NAME: IP6E_send
* RETURNS : RTCS_OK or error code.
* DESCRIPTION: Sends an IPv6 packet.
*************************************************************************/
uint_32 IP6E_send( IP_IF_PTR if_ptr     /* [IN] the IP interface structure */,
                   RTCSPCB_PTR rtcs_pcb /* [IN] the packet to send */,
                   in6_addr *src        /* [IN] the next-hop source address */,
                   in6_addr *dest       /* [IN] the next-hop destination address */)
{ 
    uchar       enetdest[6];
    uint_32     error;
    uchar       *enetdest_ptr;
   

    IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_TOTAL++);
    
    /********************************************
     * Multicast.
     ********************************************/
    if (IN6_IS_ADDR_MULTICAST(dest))  
    {
        IP6E_ADDR_MC(enetdest, dest);
        
        enetdest_ptr = (uchar *)enetdest;
        
        IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_MULTICAST++);
    } 
    /********************************************
     * Unicast.
     ********************************************/
    else 
    {
        ND6_NEIGHBOR_ENTRY_PTR neighbor;
        
        /* Possible redirection.*/
        nd6_redirect_addr(if_ptr, &dest);

        /* Check Neigbor cache.*/
        neighbor = nd6_neighbor_cache_get(if_ptr, dest);
           
        /* RFC4861 7.2.2: When a node has a unicast packet to send to a neighbor, but does not
         * know the neighbor’s link-layer address, it performs address resolution.
         * For multicast-capable interfaces, this entails creating a 
         * Neighbor Cache entry in the INCOMPLETE state and transmitting a
         * Neighbor Solicitation message targeted at the neighbor. The
         * solicitation is sent to the solicited-node multicast address
         * corresponding to the target address.
         */
        if(neighbor == NULL)
        {
            /* RFC4861 7.2.Address resolution is performed only on addresses that are determined to be
             * on-link and for which the sender does not know the corresponding link-layer address.
             * Address resolution is never performed on multicast addresses.*/
             
            if(nd6_addr_is_onlink(if_ptr, dest) == TRUE)
            /* Destimnation is ON local-link.*/
            {
                /* Creating a Neighbor Cache entry in the INCOMPLETE state. */
                neighbor =  nd6_neighbor_cache_add(if_ptr, dest, NULL, ND6_NEIGHBOR_STATE_INCOMPLETE);
                
                neighbor->state_time = RTCS_time_get();
                neighbor->solicitation_send_counter = 0;
                IN6_ADDR_COPY(src, &neighbor->solicitation_src_ip_addr); /* Save src address for later usage.*/
                
                /* AR: Transmitting a Neighbor Solicitation message targeted at the neighbor.*/
                nd6_neighbor_solicitation_send(if_ptr, src, NULL /* NULL for AR */, dest);
            }
            /* Destination is OFF local-link.*/
            else
            {
                /* Try to use the router, if exists.*/
                neighbor = nd6_default_router_get(if_ptr);

                if(neighbor == NULL)
                /* No Router exists.*/
                {
                #if RTCSCFG_DEBUG_MSG
                    printf("ETH6:Nor Router - DROP\n");
                #endif    
                    
                    IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_DISCARDED++);
                    RTCSPCB_free(rtcs_pcb);
                    return RTCSERR_IP_UNREACH;   
                }

                dest = &neighbor->ip_addr;   /* Chage destination address to the router one */                
            }
        }

        /* Link -layer address is not initialized.*/
        if((neighbor->state != ND6_NEIGHBOR_STATE_INCOMPLETE) 
            && (neighbor->ll_addr[0]==0) && (neighbor->ll_addr[1]==0) && (neighbor->ll_addr[2]==0)
            && (neighbor->ll_addr[3]==0) && (neighbor->ll_addr[4]==0) && (neighbor->ll_addr[5]==0) )
        {
            neighbor->state = ND6_NEIGHBOR_STATE_INCOMPLETE;
            neighbor->state_time = RTCS_time_get();
            neighbor->solicitation_send_counter = 0;
            IN6_ADDR_COPY(src, &neighbor->solicitation_src_ip_addr); /* Save src address for later usage.*/
            /* AR: Transmitting a Neighbor Solicitation message targeted at the neighbor.*/
            nd6_neighbor_solicitation_send(if_ptr, src, NULL /* NULL for AR */, dest);
        }
        
        if(neighbor->state == ND6_NEIGHBOR_STATE_INCOMPLETE)
        /* Queue packet for later transmit.
         */
        {
            nd6_neighbor_enqueue_waiting_pcb(neighbor, rtcs_pcb);
            
            return RTCS_OK;
        }
        
        if(neighbor->state == ND6_NEIGHBOR_STATE_STALE)
        /* RFC4861 7.3.3: The first time a node sends a packet to a neighbor whose entry is
         * STALE, the sender changes the state to DELAY and sets a timer to
         * expire in DELAY_FIRST_PROBE_TIME seconds.
         */
        {
            neighbor->state = ND6_NEIGHBOR_STATE_DELAY;
            neighbor->state_time = RTCS_time_get();
        }
        
        /* Get destination MAC/HW address.*/
        enetdest_ptr = (uchar *)neighbor->ll_addr;
        
        IF_IPIF_STATS_ENABLED(if_ptr->STATS.ST_TX_UNICAST++);
    }
    
    /* Send message via Ethernet.
     */
    error = IPE_send(if_ptr, rtcs_pcb, ENETPROT_IP6, enetdest_ptr);
    RTCSLOG_FNX2(RTCSLOG_FN_IPE_send_IP, error);
 
    return error;
}

#endif /* RTCSCFG_ENABLE_IP6 */


