#ifndef __ipcfg_h__
#define __ipcfg_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: ipcfg.h$
* $Version : 3.8.23.0$
* $Date    : Aug-15-2012$
*
* Comments:
*
*   Definitions for the IP config layer.
*
*   RTCSCFG_ENABLE_GATEWAYS must be set to work with gateways
*   RTCSCFG_IPCFG_ENABLE_DNS must be set to enable DNS resolving
*   RTCSCFG_IPCFG_ENABLE_DHCP must be set for DHCP
*   RTCSCFG_IPCFG_ENABLE_BOOT must be set to enable tftp/boot names processing
*
*END************************************************************************/


#include <enet.h>
#include <rtcs.h>


/* configuration defines */
#define IPCFG_DEBUG_LEVEL           0

#define IPCFG_MAX_NAK_RETRIES       6
#define IPCFG_MAX_AUTOIP_ATTEMPTS   256

#define IPCFG_TASK_NAME             "IPCFG_TASK"
#define IPCFG_TASK_STACK_SIZE       1000


#define IPCFG_DEVICE_COUNT  BSP_ENET_DEVICE_COUNT

/* error codes - backward compatibility */
#define IPCFG_ERROR_OK              IPCFG_OK
#define IPCFG_ERROR_BUSY            RTCSERR_IPCFG_BUSY
#define IPCFG_ERROR_DEVICE_NUMBER   RTCSERR_IPCFG_DEVICE_NUMBER
#define IPCFG_ERROR_INIT            RTCSERR_IPCFG_INIT
#define IPCFG_ERROR_BIND            RTCSERR_IPCFG_BIND


/* types */
typedef enum ipcfg_state
{
    IPCFG_STATE_INIT = 0,
    IPCFG_STATE_UNBOUND,
    IPCFG_STATE_BUSY,
    IPCFG_STATE_STATIC_IP,
    IPCFG_STATE_DHCP_IP,
    IPCFG_STATE_AUTO_IP,
    IPCFG_STATE_DHCPAUTO_IP,
    IPCFG_STATE_BOOT
} IPCFG_STATE;

typedef struct ipcfg_ip_address_data
{
    _ip_address ip;
    _ip_address mask;
    _ip_address gateway;
} IPCFG_IP_ADDRESS_DATA, _PTR_ IPCFG_IP_ADDRESS_DATA_PTR;

/* ipcfg general API */
uint_32         ipcfg_init_device (uint_32, _enet_address);
uint_32         ipcfg_release_device(uint_32);
uint_32         ipcfg_init_interface(uint_32, _rtcs_if_handle);
uint_32         ipcfg_release_interface(uint_32);

/* immediate action API */
uint_32         ipcfg_unbind (uint_32);
uint_32         ipcfg_bind_staticip (uint_32, IPCFG_IP_ADDRESS_DATA_PTR);
uint_32         ipcfg_bind_autoip (uint_32, IPCFG_IP_ADDRESS_DATA_PTR);
uint_32         ipcfg_bind_dhcp (uint_32, boolean);
uint_32         ipcfg_poll_dhcp (uint_32, boolean, IPCFG_IP_ADDRESS_DATA_PTR);
uint_32         ipcfg_bind_boot (uint_32);
uint_32         ipcfg_bind_dhcp_wait (uint_32, boolean, IPCFG_IP_ADDRESS_DATA_PTR);

/* monitoring-task API */
uint_32         ipcfg_task_create (uint_32, uint_32);
void            ipcfg_task_destroy (boolean);
boolean         ipcfg_task_status (void);
boolean         ipcfg_task_poll (void);

/* general information retrieval */
uint_32         ipcfg_get_device_number(_rtcs_if_handle);
uint_32         ipcfg_add_interface(uint_32, _rtcs_if_handle);
uint_32         ipcfg_del_interface (uint_32);
_rtcs_if_handle ipcfg_get_ihandle (uint_32);
boolean         ipcfg_get_mac (uint_32, _enet_address);
const char *    ipcfg_get_state_string (IPCFG_STATE);
IPCFG_STATE     ipcfg_get_state (uint_32);
boolean         ipcfg_phy_registers(uint_32,uint_32,uint_32_ptr);
IPCFG_STATE     ipcfg_get_desired_state (uint_32);
boolean         ipcfg_get_link_active (uint_32);
_ip_address     ipcfg_get_dns_ip (uint_32, uint_32);
boolean         ipcfg_add_dns_ip (uint_32, _ip_address);
boolean         ipcfg_del_dns_ip (uint_32, _ip_address);
boolean         ipcfg_get_ip (uint_32, IPCFG_IP_ADDRESS_DATA_PTR);
_ip_address     ipcfg_get_tftp_serveraddress (uint_32);
uchar_ptr       ipcfg_get_tftp_servername (uint_32);
uchar_ptr       ipcfg_get_boot_filename (uint_32);

#define IPCFG_INC_IP(ip, mask, val)     ((ip & mask) | ((ip + val + ((((ip + val) & 0xff) == 0xff) ? 2 : 0)) & ~mask))

/******************************************************************
 * Possible states for the address of an interface.
 ******************************************************************/
typedef enum
{
    IP6_ADDR_STATE_NOT_USED = 0,    /* Not used.*/
    IP6_ADDR_STATE_TENTATIVE = 1,	/* Tentative address - (RFC4862) an address whose uniqueness on a link is being
                                     * verified, prior to its assignment to an interface. A tentative
                                     * address is not considered assigned to an interface in the usual
                                     * sense. An interface discards received packets addressed to a
                                     * tentative address, but accepts Neighbor Discovery packets related
                                     * to Duplicate Address Detection for the tentative address.
                                     */
    IP6_ADDR_STATE_PREFERRED = 2 	/* Prefered address - (RFC4862) an address assigned to an interface whose use by
                                     * upper-layer protocols is unrestricted. Preferred addresses may be
                                     * used as the source (or destination) address of packets sent from
                                     * (or to) the interface.
                                     */
} ip6_if_addr_state;

/******************************************************************
* Structures
*******************************************************************/
typedef struct ipcfg6_bind_addr_data
{
    in6_addr            ip_addr;    /* is IP address if "type" is MANUAL, 
                                     * or is prefix/64 if "type" is AUTOCONFIGURABLE"*/
    ip6_if_addr_type    ip_addr_type;
} IPCFG6_BIND_ADDR_DATA, _PTR_ IPCFG6_BIND_ADDR_DATA_PTR;

typedef struct ipcfg6_unbind_addr_data
{
    in6_addr        ip_addr;
} IPCFG6_UNBIND_ADDR_DATA, _PTR_ IPCFG6_UNBIND_ADDR_DATA_PTR;

typedef struct ipcfg6_get_addr_data
{
    in6_addr            ip_addr;        /* Address.*/
    ip6_if_addr_state   ip_addr_state;  /* Address current state.*/
    ip6_if_addr_type    ip_addr_type;   /* Address type.*/
} IPCFG6_GET_ADDR_DATA, _PTR_ IPCFG6_GET_ADDR_DATA_PTR;


/******************************************************************
* Function Prototypes
*******************************************************************/
/* Immediate action API. */
uint_32         ipcfg6_unbind_addr (uint_32, IPCFG6_UNBIND_ADDR_DATA_PTR);
uint_32         ipcfg6_bind_addr (uint_32, IPCFG6_BIND_ADDR_DATA_PTR);

/* General information retrieval. */
uint_32 ipcfg6_get_addr_info_n ( uint_32 device, uint_32 n /*>0*/, IPCFG6_GET_ADDR_DATA_PTR data/*out*/);
uint_32 ipcfg6_get_addr_info (uint_32 device, in6_addr *addr /*in*/, IPCFG6_GET_ADDR_DATA_PTR data/*out*/);
uint_32 ipcfg6_get_device_scope_id( uint_32 device /* in */ );

#endif
/* EOF */
