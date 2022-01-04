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
* $FileName: ipcfg.c$
* $Version : 3.8.36.0$
* $Date    : Sep-19-2012$
*
* Comments:
*
*   High level IP config functions and task.
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <enet.h>
#include <rtcs.h>
#include <dns.h> 
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"
#include "ipcfg.h"
#include "ipcfg_prv.h"

#ifdef BSP_ENET_DEVICE_COUNT
#if  (BSP_ENET_DEVICE_COUNT > 0) 


#if (RTCSCFG_IPCFG_ENABLE_DHCP || RTCSCFG_IPCFG_ENABLE_BOOT) && (! RTCSCFG_ENABLE_UDP)
#error "Please set RTCSCFG_ENABLE_UDP to 1 to enable DHCP or BOOT and recompile RTCS again!"
#endif


/* hidden global data */
static uchar            ipcfg_empty_string[]                = "";
static const char       ipcfg_task_name[]                   = IPCFG_TASK_NAME;
static boolean          ipcfg_task_run                      = FALSE;
IPCFG_CONTEXT    ipcfg_data[IPCFG_DEVICE_COUNT]  = {0};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipcfg_get_device_number()
* Returned Value  : device number or -1
* Comments        : Returns corresponding device number for given interface.
*
*END*-----------------------------------------------------------------*/

uint_32 ipcfg_get_device_number
    (
        _rtcs_if_handle ihandle
    )
{
    _mqx_int i;
   
    for (i = 0; i < IPCFG_DEVICE_COUNT; i++) {
        if (ipcfg_data[i].ihandle == ihandle)
            break;
    }
   
    return (i < IPCFG_DEVICE_COUNT) ? i : -1;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipcfg_add_interface()
* Returned Value  : device number or -1
* Comments        : Add new interface and returns corresponding device number.
*
*END*-----------------------------------------------------------------*/

uint_32 ipcfg_add_interface
    (
        uint_32 dev_num,
        _rtcs_if_handle ihandle
    )
{
    _mqx_int i = IPCFG_DEVICE_COUNT;
    
    if (dev_num != (uint_32)-1) {
        if (dev_num < IPCFG_DEVICE_COUNT && ipcfg_data[dev_num].ihandle == NULL) {
            ipcfg_data[dev_num].ihandle = ihandle;
            i = dev_num;
        }
    }
    else {
        for (i = 0; i < IPCFG_DEVICE_COUNT; i++) {
            if (ipcfg_data[i].ihandle == NULL) {
                ipcfg_data[i].ihandle = ihandle;
                break;
            }
        }
    }
   
    return (i < IPCFG_DEVICE_COUNT) ? i : -1;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipcfg_del_interface()
* Returned Value  : ipcfg error code.
* Comments        : Delete interface defined by device number.
*
*END*-----------------------------------------------------------------*/
uint_32 ipcfg_del_interface (uint_32 dev_num)
{
    if (dev_num < IPCFG_DEVICE_COUNT)
    {
        ipcfg_data[dev_num].actual_state = IPCFG_STATE_INIT;
        ipcfg_data[dev_num].ihandle = NULL;
        return IPCFG_OK;
    }
    else
        return RTCSERR_IPCFG_DEVICE_NUMBER;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipcfg_get_ihandle()
* Returned Value  : interface handle or NULL
* Comments        : Returns interface handle for given device number.
*
*END*-----------------------------------------------------------------*/

_rtcs_if_handle ipcfg_get_ihandle 
    (
        uint_32 device
    )
{
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) return ipcfg_data[device].ihandle;
    }
    return NULL;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipcfg_get_mac()
* Returned Value  : TRUE and mac address if successful, FALSE otherwise
* Comments        : Returns ethernet mac address for given device number.
*
*END*-----------------------------------------------------------------*/

boolean ipcfg_get_mac 
    (
        uint_32         device,
        _enet_address   mac
    )
{
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) 
        {
            ENET_get_address (((IP_IF_PTR)(ipcfg_data[device].ihandle))->HANDLE, mac);
            return TRUE;
        }
        else
        {
            ENET_get_mac_address (device, 0, mac);
            return TRUE;
        }
    }
    return FALSE;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipcfg_get_state_string()
* Returned Value  : state name string or NULL
* Comments        : Returns const char_ptr to name of given state.
*
*END*-----------------------------------------------------------------*/

const char * ipcfg_get_state_string (IPCFG_STATE state)
{
    static const char_ptr ipcfg_states[] = 
    {
        "INIT",         //"init",
        "UNBOUND",      //"unbound",
        "BUSY",         //"busy",
        "MANUAL",       //"staticip",
        "DHCPNOAUTO",   //"dhcpnoauto",
        "AUTOIP",       //"autoip",
        "DHCP",         //"dhcp",
        "BOOT"          //"boot"
    };

    if (state < sizeof (ipcfg_states))
    {
        return ipcfg_states[state];
    }
    return NULL;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_get_state()
* Returned Value : actual ipcfg state
* Comments       : Returns ipcfg state enum element.
*
*END--------------------------------------------------------------------*/

IPCFG_STATE ipcfg_get_state 
    (
        uint_32 device
    )
{
    IPCFG_STATE state = IPCFG_STATE_INIT;
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT)
        {
            if (! _lwsem_poll (&(ipcfg_data[device].request_semaphore))) return IPCFG_STATE_BUSY;
            state = ipcfg_data[device].actual_state;
            _lwsem_post (&(ipcfg_data[device].request_semaphore));
        }
    }
    return state;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_get_last_state()
* Returned Value : last ipcfg state
* Comments       : Returns ipcfg state enum element.
*
*END--------------------------------------------------------------------*/

IPCFG_STATE ipcfg_get_desired_state 
    (
        uint_32 device
    )
{
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) return ipcfg_data[device].desired_state;
    }
    return IPCFG_STATE_INIT;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_get_link_active()
* Returned Value : TRUE if link active, FALSE otherwise
* Comments       : Returns actual link state.
*
*END--------------------------------------------------------------------*/

boolean ipcfg_get_link_active 
    (
        uint_32 device
    )
{
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT)
            return ENET_link_status (((IP_IF_PTR)(ipcfg_data[device].ihandle))->HANDLE);
    }
    return FALSE;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_phy_registers()
* Returned Value : TRUE if link active, FALSE otherwise
* Comments       : Returns actual link state.
*
*END--------------------------------------------------------------------*/

boolean ipcfg_phy_registers 
    (
        uint_32     device,
        uint_32     num_regs,
        uint_32_ptr regs_ptr
    )
{
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT)
            return ENET_phy_registers(((IP_IF_PTR)(ipcfg_data[device].ihandle))->HANDLE, num_regs,regs_ptr);
    }
    return FALSE;
}

/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_get_dns_ip()
* Returned Value : dns IP address
* Comments       : Returns N-th DNS ip address from DNS list for given N.
*
*END--------------------------------------------------------------------*/

_ip_address ipcfg_get_dns_ip 
    (
        uint_32 device,
        uint_32 n
    )
{
    uint_32 result = 0;

#if RTCSCFG_IPCFG_ENABLE_DNS

    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) 
        {

#if RTCSCFG_ENABLE_LWDNS

            if (n == 0) result = LWDNS_server_ipaddr;

#else

            DNS_SLIST_STRUCT _PTR_ temp_ptr;

            for (temp_ptr = DNS_First_Local_server; (temp_ptr != NULL) && (n != 0); temp_ptr = temp_ptr->NEXT) n--;
            if (temp_ptr != NULL) 
            {
                result = temp_ptr->IPADDR;
            }

#endif

        }
    }

#endif

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_add_dns_ip()
* Returned Value : TRUE if successful, FALSE otherwise
* Comments       : Adds/updates DNS ip address info inside DNS list and
*                  starts the DNS machine, if not running already.
*
*END--------------------------------------------------------------------*/

boolean ipcfg_add_dns_ip 
    (
        uint_32     device,
        _ip_address address
    )
{
    boolean result = FALSE;

#if RTCSCFG_IPCFG_ENABLE_DNS

    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) 
        {

#if RTCSCFG_ENABLE_LWDNS

            LWDNS_server_ipaddr = address;
            result = TRUE;

#else
            DNS_CONTROL_STRUCT _PTR_ control_ptr = RTCS_getcfg(DNS);
            DNS_SLIST_STRUCT slist_entry;

            if (control_ptr == NULL)
            {
                DNS_init();
            }

            DNS_init_slist_entry (&slist_entry);
            slist_entry.IPADDR = address;
            if (DNS_add_slist_entry (&slist_entry) == DNS_OK) 
            {
                result = TRUE;
            }

#endif

        }
    }

#endif

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_del_dns_ip()
* Returned Value : TRUE if successful, FALSE otherwise
* Comments       : Removes given address from DNS list, if exists.
*
*END--------------------------------------------------------------------*/

boolean ipcfg_del_dns_ip 
    (
        uint_32     device,
        _ip_address address
    )
{
    boolean result = FALSE;

#if RTCSCFG_IPCFG_ENABLE_DNS

    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) 
        {
        
#if RTCSCFG_ENABLE_LWDNS

            if (LWDNS_server_ipaddr == address) 
            {
                LWDNS_server_ipaddr = 0;
                result = TRUE;
            }
        
#else

            DNS_remove_slist_entry (address);
            result = TRUE;

#endif

        }
    }

#endif

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_get_ip()
* Returned Value : TRUE and IP, mask and gateway, if successful
* Comments       : Returns actual ip information from DHCP response, if any.
*
*END--------------------------------------------------------------------*/

boolean ipcfg_get_ip 
    (
        uint_32                     device, 
        IPCFG_IP_ADDRESS_DATA_PTR   data
    )
{
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) 
        {
            *data = ipcfg_data[device].actual_ip_data;
            return TRUE;
        }
    }
    return FALSE;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_get_tftp_serveraddress()
* Returned Value : _ip_address
* Comments       : Returns TFTP serveraddress, if any.
*
*END--------------------------------------------------------------------*/

_ip_address ipcfg_get_tftp_serveraddress 
    (
        uint_32 device
    )
{
#if RTCSCFG_IPCFG_ENABLE_BOOT
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) return ipcfg_data[device].tftp_serveraddress;
    }
#endif
    return 0;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_get_tftp_servername()
* Returned Value : pointer to string servername
* Comments       : Returns TFTP servername, if any.
*
*END--------------------------------------------------------------------*/

uchar_ptr ipcfg_get_tftp_servername 
    (
        uint_32 device
    )
{
#if RTCSCFG_IPCFG_ENABLE_BOOT
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) return ipcfg_data[device].tftp_servername;
    }
#endif
    return ipcfg_empty_string;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_get_boot_filename()
* Returned Value : pointer to string filename
* Comments       : Returns boot filename, if any.
*
*END--------------------------------------------------------------------*/

uchar_ptr ipcfg_get_boot_filename 
    (
        uint_32 device
    )
{
#if RTCSCFG_IPCFG_ENABLE_BOOT
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) return ipcfg_data[device].boot_filename;
    }
#endif
    return ipcfg_empty_string;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_unbind_internal_actual()
* Returned Value : ipcfg error code
* Comments       : Actual unbind process.
*
*END--------------------------------------------------------------------*/

static uint_32 ipcfg_unbind_internal_actual
    (
        uint_32 device
    )
{
    if (ipcfg_data[device].actual_ip_data.ip != 0) 
    {
        if ((ipcfg_data[device].actual_state == IPCFG_STATE_DHCP_IP) || (ipcfg_data[device].actual_state == IPCFG_STATE_DHCPAUTO_IP)) 
        {
#if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
            DHCPCLNT_release (ipcfg_data[device].ihandle);
#endif
        } 
        else if ((ipcfg_data[device].actual_state == IPCFG_STATE_STATIC_IP) || (ipcfg_data[device].actual_state == IPCFG_STATE_AUTO_IP) || (ipcfg_data[device].actual_state == IPCFG_STATE_BOOT))
        {
            RTCS_if_unbind (ipcfg_data[device].ihandle, ipcfg_data[device].actual_ip_data.ip);
        }
    }

#if RTCSCFG_ENABLE_GATEWAYS
    if (ipcfg_data[device].actual_ip_data.gateway != 0) 
    {
        RTCS_gate_remove (ipcfg_data[device].actual_ip_data.gateway, INADDR_ANY, INADDR_ANY);
    }
#endif

#if RTCSCFG_IPCFG_ENABLE_DNS
    if (ipcfg_data[device].dns != 0)
    {
        ipcfg_del_dns_ip (device, ipcfg_data[device].dns);
    }
    ipcfg_data[device].dns = 0;
#endif

    ipcfg_data[device].actual_ip_data.ip = 0;
    ipcfg_data[device].actual_ip_data.mask = 0;
    ipcfg_data[device].actual_ip_data.gateway = 0;
#if RTCSCFG_IPCFG_ENABLE_BOOT
    ipcfg_data[device].tftp_serveraddress = 0;
    ipcfg_data[device].tftp_servername[0] = ipcfg_empty_string[0];
    ipcfg_data[device].boot_filename[0] = ipcfg_empty_string[0];
#endif
#if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
    ipcfg_data[device].dhcp_retries = 0;
#endif
    ipcfg_data[device].actual_state = IPCFG_STATE_UNBOUND;
    return IPCFG_OK;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_unbind_internal()
* Returned Value : ipcfg error code
* Comments       : Control semaphore unprotected unbind.
*
*END--------------------------------------------------------------------*/

static uint_32 ipcfg_unbind_internal
    (
        uint_32 device
    )
{
    uint_32 result;
    
    if (! _lwsem_poll (&(ipcfg_data[device].request_semaphore))) return RTCSERR_IPCFG_BUSY;

    result = ipcfg_unbind_internal_actual (device);

    _lwsem_post (&(ipcfg_data[device].request_semaphore));

    return result;  
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_unbind()
* Returned Value : ipcfg error code
* Comments       : Releases any ip information associated with given device.
*
*END--------------------------------------------------------------------*/

uint_32 ipcfg_unbind
    (
        uint_32 device
    )
{
    uint_32 result;

    if (device >= IPCFG_DEVICE_COUNT) return RTCSERR_IPCFG_DEVICE_NUMBER;

    if (ipcfg_data[device].actual_state == IPCFG_STATE_INIT) return RTCSERR_IPCFG_INIT;
    
    if (! _lwsem_poll (&(ipcfg_data[device].control_semaphore))) return RTCSERR_IPCFG_BUSY;

    result = ipcfg_unbind_internal (device);

    if (result == IPCFG_OK)
    {
        ipcfg_data[device].desired_ip_data = ipcfg_data[device].actual_ip_data;
        ipcfg_data[device].desired_state = ipcfg_data[device].actual_state;
    }

    _lwsem_post (&(ipcfg_data[device].control_semaphore));

    return result;  
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_bind_staticip_internal()
* Returned Value : ipcfg error code
* Comments       : Actual static bind process.
*
*END--------------------------------------------------------------------*/

static uint_32 ipcfg_bind_staticip_internal 
    (
        uint_32                     device,
        IPCFG_IP_ADDRESS_DATA_PTR   static_ip_data
    )
{
    uint_32 result;

    if (! _lwsem_poll (&(ipcfg_data[device].request_semaphore))) return RTCSERR_IPCFG_BUSY;

    ipcfg_unbind_internal_actual (device);
    
    if (static_ip_data == NULL)
    {
        static_ip_data = &(ipcfg_data[device].desired_ip_data);
    }

    result = RTCSERR_IPCFG_BIND;
    if (RTCS_if_probe_and_bind (ipcfg_data[device].ihandle, static_ip_data->ip, static_ip_data->mask) == RTCS_OK)
    {
        ipcfg_data[device].desired_ip_data.ip = static_ip_data->ip;
        ipcfg_data[device].desired_ip_data.mask = static_ip_data->mask;
        ipcfg_data[device].desired_ip_data.gateway = static_ip_data->gateway;
        ipcfg_data[device].actual_ip_data.ip = static_ip_data->ip;
        ipcfg_data[device].actual_ip_data.mask = static_ip_data->mask;
#if RTCSCFG_ENABLE_GATEWAYS
        if (static_ip_data->gateway != 0)
        {
            if (RTCS_gate_add (static_ip_data->gateway, INADDR_ANY, INADDR_ANY) == RTCS_OK) 
            {
                ipcfg_data[device].actual_ip_data.gateway = static_ip_data->gateway;
            }
        }
#endif
        ipcfg_data[device].desired_state = IPCFG_STATE_STATIC_IP;       
        ipcfg_data[device].actual_state = ipcfg_data[device].desired_state;
        result = IPCFG_OK;
    }

    _lwsem_post (&(ipcfg_data[device].request_semaphore));

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_bind_staticip()
* Returned Value : ipcfg error code
* Comments       : Tries to bind given static ip for given device.
*
*END--------------------------------------------------------------------*/

uint_32 ipcfg_bind_staticip 
    (
        uint_32                     device,
        IPCFG_IP_ADDRESS_DATA_PTR   static_ip_data
    )
{
    uint_32 result;

    if (device >= IPCFG_DEVICE_COUNT) return RTCSERR_IPCFG_DEVICE_NUMBER;

    if (ipcfg_data[device].actual_state == IPCFG_STATE_INIT) return RTCSERR_IPCFG_INIT;

    if (! _lwsem_poll (&(ipcfg_data[device].control_semaphore))) return RTCSERR_IPCFG_BUSY;

    result = ipcfg_bind_staticip_internal (device, static_ip_data);

    _lwsem_post (&(ipcfg_data[device].control_semaphore));
    
    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_bind_autoip_internal_actual()
* Returned Value : ipcfg error code
* Comments       : Actual auto bind process.
*
*END--------------------------------------------------------------------*/

static uint_32 ipcfg_bind_autoip_internal_actual
    (
        uint_32                     device,
        IPCFG_IP_ADDRESS_DATA_PTR   auto_ip_data
    )
{
    uint_32 attempts;
    uint_32 result;
    
    ipcfg_unbind_internal_actual (device);

    if (auto_ip_data == NULL)
    {
        auto_ip_data = &(ipcfg_data[device].desired_ip_data);
    }

    result = RTCSERR_IPCFG_BIND;
    for (attempts = 0; attempts < IPCFG_MAX_AUTOIP_ATTEMPTS; attempts++)
    {
        if (RTCS_if_probe_and_bind (ipcfg_data[device].ihandle, auto_ip_data->ip, auto_ip_data->mask) == RTCS_OK)
        {
            ipcfg_data[device].desired_ip_data.ip = auto_ip_data->ip;
            ipcfg_data[device].desired_ip_data.mask = auto_ip_data->mask;
            ipcfg_data[device].desired_ip_data.gateway = auto_ip_data->gateway;
            ipcfg_data[device].actual_ip_data.ip = auto_ip_data->ip;
            ipcfg_data[device].actual_ip_data.mask = auto_ip_data->mask;
#if RTCSCFG_ENABLE_GATEWAYS
            if (auto_ip_data->gateway != 0)
            {   
                if (RTCS_gate_add (auto_ip_data->gateway, INADDR_ANY, INADDR_ANY) == RTCS_OK) 
                {
                    ipcfg_data[device].actual_ip_data.gateway = auto_ip_data->gateway;
                }
            }
#endif
            ipcfg_data[device].desired_state = IPCFG_STATE_AUTO_IP;
            ipcfg_data[device].actual_state = ipcfg_data[device].desired_state;
            result = IPCFG_OK;
            break;
        } 
        do
        {
            auto_ip_data->ip = (auto_ip_data->ip & auto_ip_data->mask) | ((auto_ip_data->ip + 1) & (~ auto_ip_data->mask));
        } while (auto_ip_data->ip & (~ auto_ip_data->mask) == 0);
    }

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_bind_autoip_internal()
* Returned Value : ipcfg error code
* Comments       : Control semaphore unprotected auto bind.
*
*END--------------------------------------------------------------------*/

static uint_32 ipcfg_bind_autoip_internal
    (
        uint_32                     device,
        IPCFG_IP_ADDRESS_DATA_PTR   auto_ip_data
    )
{
    uint_32 result;
    
    if (! _lwsem_poll (&(ipcfg_data[device].request_semaphore))) return RTCSERR_IPCFG_BUSY;

    result = ipcfg_bind_autoip_internal_actual (device, auto_ip_data);

    _lwsem_post (&(ipcfg_data[device].request_semaphore));

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_bind_autoip()
* Returned Value : ipcfg error code
* Comments       : Tries to bind given ip address data for given device and 
*                  continues incrementing ip until bound or maximum tries reached.
*
*END--------------------------------------------------------------------*/

uint_32 ipcfg_bind_autoip 
    (
        uint_32                     device,
        IPCFG_IP_ADDRESS_DATA_PTR   auto_ip_data
    )
{
    uint_32 result;
    
    if (device >= IPCFG_DEVICE_COUNT) return RTCSERR_IPCFG_DEVICE_NUMBER;

    if (ipcfg_data[device].actual_state == IPCFG_STATE_INIT) return RTCSERR_IPCFG_INIT;

    if (! _lwsem_poll (&(ipcfg_data[device].control_semaphore))) return RTCSERR_IPCFG_BUSY;
    
    result = ipcfg_bind_autoip_internal (device, auto_ip_data);

    _lwsem_post (&(ipcfg_data[device].control_semaphore));

    return result;
}


#if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_dhcp_choice_func()
* Returned Value : 0 == not acceptable
* Comments       : This function is used to choose if a DHCPOFFER is
*                  acceptable. If it is, the Client will request the
*                  lease with the parameters given.
*
*END--------------------------------------------------------------------*/

static int_32 ipcfg_dhcp_choice_func
    (
        uchar_ptr buffer_ptr,
        uint_32 buffer_size
    )
{
#if IPCFG_DEBUG_LEVEL >= 1
    printf ("DHCP choice func.\n");
#endif
    return 1;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_bind_dhcp_func()
* Returned Value : void
* Comments       : This function is called when the DHCP interface is bound
*                  to inform the application that a network connection has
*                  been established. Can be used to trigger an event, etc..
*
*END--------------------------------------------------------------------*/

static void ipcfg_bind_dhcp_func
    (
        uchar_ptr buffer_ptr,
        uint_32 buffer_size,
        _rtcs_if_handle ihandle
    )
{
    uchar_ptr           optptr;
    uint_32             optlen;
    DHCP_HEADER_PTR     dhcp_ptr = (DHCP_HEADER_PTR)buffer_ptr;
    uint_32             device = ipcfg_get_device_number (ihandle);

    if (device >= IPCFG_DEVICE_COUNT)
    {
#if IPCFG_DEBUG_LEVEL >= 1
        printf ("DHCP bind func - wrong device number.\n");
#endif
        return;
    } 

    if ((buffer_ptr == NULL) || (buffer_size == 0)) 
    {
#if IPCFG_DEBUG_LEVEL >= 1
        printf ("DHCP bind func - lease not obtained.\n");
#endif
    }
    else 
    {
#if IPCFG_DEBUG_LEVEL >= 1
        printf ("DHCP bind func - success.\n");
#endif

        ipcfg_data[device].actual_ip_data.ip = ntohl (dhcp_ptr->YIADDR);

        optptr = DHCPCLNT_find_option (buffer_ptr, buffer_size, DHCPOPT_MASK);
        if (optptr) 
        {
            optptr++;
            optlen = ntohc (optptr);
            optptr++;
            if (optlen >= 4)
            {
                ipcfg_data[device].actual_ip_data.mask = ntohl  (optptr);
            }
        } 

#if RTCSCFG_ENABLE_GATEWAYS
        optptr = DHCPCLNT_find_option (buffer_ptr, buffer_size, DHCPOPT_ROUTER);
        if (optptr) {
            optptr++;
            optlen = ntohc (optptr);
            optptr++;
            if (optlen >= 4) 
            {
                ipcfg_data[device].actual_ip_data.gateway = ntohl (optptr);
            }
        } 
#endif

#if RTCSCFG_IPCFG_ENABLE_DNS
        optptr = DHCPCLNT_find_option (buffer_ptr, buffer_size, DHCPOPT_DNSSERVER);
        if (optptr) 
        {
            optptr++;
            optlen = ntohc (optptr);
            optptr++;
            if (optlen >= 4) 
            {
                ipcfg_data[device].dns = ntohl (optptr);
            }
        }
#endif

#if RTCSCFG_IPCFG_ENABLE_BOOT
        optptr = DHCPCLNT_find_option (buffer_ptr, buffer_size, DHCPOPT_TFTPSERVER);
        if (optptr) 
        {
            optptr++;
            optlen = ntohc (optptr);
            optptr++;
            if (optlen >= RTCS_SNAME_LEN - 1) optlen = RTCS_SNAME_LEN - 2; 
            _mem_copy (optptr, ipcfg_data[device].tftp_servername, optlen);
            ipcfg_data[device].tftp_servername[optlen + 1] = '\0';
        } 
        else 
        {
            _mem_copy (dhcp_ptr->SNAME, ipcfg_data[device].tftp_servername, RTCS_SNAME_LEN);
            ipcfg_data[device].tftp_servername[RTCS_SNAME_LEN - 1] = '\0';
        } 

        optptr = DHCPCLNT_find_option (buffer_ptr, buffer_size, DHCPOPT_BOOTFILENAME);
        if (optptr) 
        {
            optptr++;
            optlen = ntohc (optptr);
            optptr++;
            if (optlen >= RTCS_BOOTFILE_LEN - 1) optlen = RTCS_BOOTFILE_LEN - 2; 
            _mem_copy (optptr, ipcfg_data[device].boot_filename, optlen);
            ipcfg_data[device].boot_filename[optlen + 1] = '\0';
        } 
        else
        {
            _mem_copy (dhcp_ptr->FILE, ipcfg_data[device].boot_filename, RTCS_BOOTFILE_LEN);
            ipcfg_data[device].boot_filename[RTCS_BOOTFILE_LEN - 1] = '\0';
        } 
#endif

        if (ipcfg_data[device].actual_state == IPCFG_STATE_UNBOUND) 
			ipcfg_data[device].actual_state = IPCFG_STATE_BUSY;
    }   

    if (ipcfg_data[device].actual_state == IPCFG_STATE_BUSY) 
		_lwsem_post (&(ipcfg_data[device].dhcp_response_semaphore));
} 


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_dhcp_nak_func()
* Returned Value : boolean
* Comments       : This function is called when the DHCP interface is bound
*                  to inform the application that a network connection has
*                  been established. Can be used to trigger an event, etc..
*
*END--------------------------------------------------------------------*/

static boolean ipcfg_dhcp_nak_func
    (
        uchar_ptr buffer_ptr,
        uint_32 buffer_size,
        _rtcs_if_handle ihandle
    )
{
    uint_32 device = ipcfg_get_device_number (ihandle);
#if IPCFG_DEBUG_LEVEL >= 1
    printf("DHCP nak func - ");
#endif
    if (device >= IPCFG_DEVICE_COUNT)
    {
#if IPCFG_DEBUG_LEVEL >= 1
        printf ("wrong device number.\n");
#endif
        return FALSE;
    }
    if (ipcfg_data[device].dhcp_retries++ < IPCFG_MAX_NAK_RETRIES) 
    {
#if IPCFG_DEBUG_LEVEL >= 1
        printf ("retry %d.\n", ipcfg_data[device].dhcp_retries);
#endif
        return TRUE;
    } 
    else 
    {
#if IPCFG_DEBUG_LEVEL >= 1
        printf ("abort.\n");
#endif
        return FALSE;
    }
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_dhcp_unbind_func()
* Returned Value : boolean
* Comments       : This function is called when the DHCP lease terminates.
*                  It allows the application the flexibility to decide if
*                  it wishes to re-bind the interface again or terminate the
*                  DHCP client and permanently unbind.
*
*END--------------------------------------------------------------------*/

static boolean ipcfg_dhcp_unbind_func
    (
        _rtcs_if_handle ihandle
    )
{
#if IPCFG_DEBUG_LEVEL >= 1
    printf("DHCP unbind func.\n");
#endif
    // remove gateway here
    return( TRUE );
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_dhcp_fail_func()
* Returned Value : boolean
* Comments       : This function is called when a DHCP failure is detected
*                  This is will invoked if the a NAK is recieved from the server
*
*END--------------------------------------------------------------------*/
static boolean ipcfg_dhcp_fail_func
    (
        _rtcs_if_handle ihandle
    )
{
    uint_32 device = ipcfg_get_device_number (ihandle);
#if IPCFG_DEBUG_LEVEL >= 1
    printf("DHCP fail func.\n");
#endif
    if (device < IPCFG_DEVICE_COUNT)
    {
        _lwsem_post(&(ipcfg_data[device].dhcp_response_semaphore));
    }
    return FALSE;
}
#endif


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : ipcfg_bind_dhcp_internal()
* Returned Value : ipcfg error code
* Comments       : Actual DHCP start process.
*
*END------------------------------------------------------------------*/

static uint_32 ipcfg_bind_dhcp_internal
    (
        uint_32 device,
        boolean try_auto_ip
    )
{
#if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
    DHCP_DATA_STRUCT    options;
    uchar_ptr           optptr;
    uchar               DHCP_option_array[256];
    uint_32             optlen = sizeof (DHCP_option_array);
    uchar               parm_options[] = {DHCPOPT_MASK, DHCPOPT_DNSSERVER, DHCPOPT_ROUTER, DHCPOPT_TFTPSERVER, DHCPOPT_BOOTFILENAME};
    uint_32             error = 0;
    uint_32             result = IPCFG_OK;

    if (! _lwsem_poll (&(ipcfg_data[device].request_semaphore))) return RTCSERR_IPCFG_BUSY;
    
    ipcfg_unbind_internal_actual (device);

    options.CHOICE_FUNC  = ipcfg_dhcp_choice_func;
    options.BIND_FUNC    = ipcfg_bind_dhcp_func;
    options.UNBIND_FUNC  = ipcfg_dhcp_unbind_func;
    options.FAILURE_FUNC = ipcfg_dhcp_fail_func;
    options.NAK_FUNC     = ipcfg_dhcp_nak_func;

    optptr = DHCP_option_array;

    DHCP_option_variable (&optptr, &optlen, DHCPOPT_PARAMLIST, parm_options, sizeof (parm_options));

    error = RTCS_if_bind_DHCP (ipcfg_data[device].ihandle, &options, DHCP_option_array, optptr - DHCP_option_array);
    if (error != RTCS_OK) 
    { 
        if (try_auto_ip) 
        {
            _lwsem_post(&(ipcfg_data[device].dhcp_response_semaphore));
        }
        else 
        {
            _lwsem_post (&(ipcfg_data[device].request_semaphore));
            result = RTCSERR_IPCFG_BIND;
        }
    } 

    return result;
#else
    return RTCSERR_IPCFG_BIND;
#endif
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : ipcfg_bind_dhcp()
* Returned Value : ipcfg error code
* Comments       : Starts DHCP process for given device, locks it with semaphore.
*
*END------------------------------------------------------------------*/

uint_32 ipcfg_bind_dhcp 
    (
        uint_32 device,
        boolean try_auto_ip
    )
{
    uint_32 result;

    if (device >= IPCFG_DEVICE_COUNT) return RTCSERR_IPCFG_DEVICE_NUMBER;

    if (ipcfg_data[device].actual_state == IPCFG_STATE_INIT) return RTCSERR_IPCFG_INIT;
        
    if (! _lwsem_poll (&(ipcfg_data[device].control_semaphore))) return RTCSERR_IPCFG_BUSY;

#if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
    result = ipcfg_bind_dhcp_internal (device, try_auto_ip);
#else
    result = RTCSERR_IPCFG_BIND;
#endif
    
    _lwsem_post (&(ipcfg_data[device].control_semaphore));

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_poll_dhcp_internal()
* Returned Value : ipcfg error code
* Comments       : Actual DHCP polling process.
*
*END--------------------------------------------------------------------*/

static uint_32 ipcfg_poll_dhcp_internal
    (
        uint_32                     device,
        boolean                     try_auto_ip,
        IPCFG_IP_ADDRESS_DATA_PTR   auto_ip_data
    )
{
#if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
    uint_32 result = RTCSERR_IPCFG_BUSY;

    if (_lwsem_poll (&(ipcfg_data[device].dhcp_response_semaphore)))
    {
        if (ipcfg_data[device].actual_state == IPCFG_STATE_BUSY)
        {
            result = IPCFG_OK;
            
#if RTCSCFG_IPCFG_ENABLE_DNS
            if (ipcfg_data[device].dns != 0)
            {
                if (! ipcfg_add_dns_ip (device, ipcfg_data[device].dns))
                {
                    ipcfg_data[device].dns = 0;
                }               
            }
#endif
#if RTCSCFG_ENABLE_GATEWAYS
            if (ipcfg_data[device].actual_ip_data.gateway != 0) 
            {
                if (RTCS_gate_add (ipcfg_data[device].actual_ip_data.gateway, INADDR_ANY, INADDR_ANY) != RTCS_OK)
                {
                    ipcfg_data[device].actual_ip_data.gateway = 0;
                }
            }
#endif
            if (auto_ip_data == NULL)
            {
                auto_ip_data = &(ipcfg_data[device].desired_ip_data);
            }

            ipcfg_data[device].desired_ip_data.ip = auto_ip_data->ip;
            ipcfg_data[device].desired_ip_data.mask = auto_ip_data->mask;
            ipcfg_data[device].desired_ip_data.gateway = auto_ip_data->gateway;
            if (try_auto_ip)
            {
                ipcfg_data[device].desired_state = IPCFG_STATE_DHCPAUTO_IP;
            }
            else
            {
                ipcfg_data[device].desired_state = IPCFG_STATE_DHCP_IP;
            }
            ipcfg_data[device].actual_state = ipcfg_data[device].desired_state;
        }
        else 
        {
            result = RTCSERR_IPCFG_BIND;
            if (try_auto_ip) 
            {
                result = ipcfg_bind_autoip_internal_actual (device, auto_ip_data);
                if (result == IPCFG_OK)
                {
                    ipcfg_data[device].desired_state = IPCFG_STATE_DHCPAUTO_IP;
                    ipcfg_data[device].actual_state = ipcfg_data[device].desired_state;
                }
            }
        }
        
        _lwsem_post (&(ipcfg_data[device].request_semaphore));
    }
    return result;
#else
    return RTCSERR_IPCFG_BIND;
#endif
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_poll_dhcp()
* Returned Value : ipcfg error code
* Comments       : Polls DHCP bind completion for given device, releases 
*                  the device related semaphore lock.
*
*END--------------------------------------------------------------------*/

uint_32 ipcfg_poll_dhcp 
    (
        uint_32                     device,
        boolean                     try_auto_ip,
        IPCFG_IP_ADDRESS_DATA_PTR   auto_ip_data
    )
{
    uint_32 result;

    if (device >= IPCFG_DEVICE_COUNT) return RTCSERR_IPCFG_DEVICE_NUMBER;

    if (ipcfg_data[device].actual_state == IPCFG_STATE_INIT) return RTCSERR_IPCFG_INIT;
    
    if (! _lwsem_poll (&(ipcfg_data[device].control_semaphore))) return IPCFG_STATE_BUSY;

#if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
    result = ipcfg_poll_dhcp_internal (device, try_auto_ip, auto_ip_data);
#else
    result = RTCSERR_IPCFG_BIND;
#endif

    _lwsem_post (&(ipcfg_data[device].control_semaphore));

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_bind_boot_internal()
* Returned Value : ipcfg error code
* Comments       : Control semaphore unprotected boot bind.
*
*END--------------------------------------------------------------------*/

static uint_32 ipcfg_bind_boot_internal
    (
        uint_32 device
    )
{
#if RTCSCFG_IPCFG_ENABLE_BOOT   
    BOOTP_DATA_STRUCT   boot_data;
#endif  
    uint_32             result = RTCSERR_IPCFG_BIND;
    
#if RTCSCFG_IPCFG_ENABLE_BOOT
    if (! _lwsem_poll (&(ipcfg_data[device].request_semaphore))) return RTCSERR_IPCFG_BUSY;
    
    ipcfg_unbind_internal_actual (device);

    if (RTCS_if_bind_BOOTP (ipcfg_data[device].ihandle, &boot_data) == RTCS_OK)
    {
#if RTCSCFG_BOOTP_RETURN_YIADDR
        if (boot_data.YIADDR != 0)
        {
            ipcfg_data[device].actual_ip_data.ip = boot_data.YIADDR;
            ipcfg_data[device].actual_ip_data.mask = 0;
            ipcfg_data[device].actual_ip_data.gateway = 0;
        } 
#endif
        if (boot_data.SADDR != 0)
        {
            ipcfg_data[device].tftp_serveraddress = boot_data.SADDR;
        }

        if (boot_data.SNAME[0] != '\0')
        {
            _mem_copy (boot_data.SNAME, ipcfg_data[device].tftp_servername, RTCS_SNAME_LEN);
            ipcfg_data[device].tftp_servername[RTCS_SNAME_LEN - 1] = '\0';
        }
                     
        if (boot_data.BOOTFILE[0] != '\0')
        {
            _mem_copy (boot_data.BOOTFILE, ipcfg_data[device].boot_filename, RTCS_BOOTFILE_LEN);
            ipcfg_data[device].boot_filename[RTCS_BOOTFILE_LEN - 1] = '\0';
        }
    
        ipcfg_data[device].desired_state = IPCFG_STATE_BOOT;
        ipcfg_data[device].actual_state = ipcfg_data[device].desired_state;
        result = IPCFG_OK;
    }

    _lwsem_post (&(ipcfg_data[device].request_semaphore));
#endif

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_bind_boot()
* Returned Value : ipcfg error code
* Comments       : Tries to bind via boot protocol for given device.
*
*END--------------------------------------------------------------------*/

uint_32 ipcfg_bind_boot 
    (
        uint_32 device
    )
{
    uint_32 result;
    
    if (device >= IPCFG_DEVICE_COUNT) return RTCSERR_IPCFG_DEVICE_NUMBER;

    if (ipcfg_data[device].actual_state == IPCFG_STATE_INIT) return RTCSERR_IPCFG_INIT;

    if (! _lwsem_poll (&(ipcfg_data[device].control_semaphore))) return RTCSERR_IPCFG_BUSY;
    
    result = ipcfg_bind_boot_internal (device);

    _lwsem_post (&(ipcfg_data[device].control_semaphore));

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_bind_dhcp_wait()
* Returned Value : ipcfg error code
* Comments       : Tries to bind via dhcp (followed by auto ip) and waits for it.
*
*END----------------------------------------------------------------*/

uint_32 ipcfg_bind_dhcp_wait 
    (
        uint_32                     device, 
        boolean                     try_auto_ip,
        IPCFG_IP_ADDRESS_DATA_PTR   auto_ip_data
    )
{
    uint_32 result = IPCFG_OK;

    do 
    {
        if (result == RTCSERR_IPCFG_BUSY) _time_delay (200);
        result = ipcfg_bind_dhcp (device, try_auto_ip);
    } while (result == RTCSERR_IPCFG_BUSY);
    if (result != IPCFG_OK) return result;
    do 
    {
        _time_delay (200);
        result = ipcfg_poll_dhcp (device, try_auto_ip, auto_ip_data);
    } while (result == RTCSERR_IPCFG_BUSY);
    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_task_poll()
* Returned Value : TRUE if bind process finished (stable), FALSE otherwise (must call it again)
* Comments       : One step of the ethernet link/bind monitoring task.
*
*END----------------------------------------------------------------*/

boolean ipcfg_task_poll (void)
{
    IPCFG_STATE     actual_state;
    IPCFG_STATE     desired_state;
    uint_32         error;
    uint_32         i;
    boolean         link;
    boolean         can_finish = TRUE;

    // go through all ethernet devices
    for (i = 0; i < IPCFG_DEVICE_COUNT; i++)
    {
        // check if current device interface is initialized
        if (ipcfg_data[i].actual_state != IPCFG_STATE_INIT)
        {
            link = ipcfg_get_link_active (i);
            
            // take control, outside binds are discarded for the moment, or skip this otherwise
            if (_lwsem_poll (&(ipcfg_data[i].control_semaphore))) 
            {
                // check link and bind status
                actual_state = ipcfg_get_state (i);
                desired_state = ipcfg_get_desired_state (i);
                if ((link) && (actual_state != desired_state)) 
                {
                    // link is up but bind is not finished
                    switch (desired_state)
                    {
                        case IPCFG_STATE_UNBOUND:
                            ipcfg_unbind_internal (i);
                            break;
                        case IPCFG_STATE_STATIC_IP:
                            ipcfg_bind_staticip_internal (i, NULL);
                            break;
                        case IPCFG_STATE_DHCP_IP:
                            error = ipcfg_bind_dhcp_internal (i, FALSE);
                            if ((error == IPCFG_OK) || (error == RTCSERR_IPCFG_BUSY))
                            {
                                ipcfg_poll_dhcp_internal (i, FALSE, NULL);
                            }
                            break;
                        case IPCFG_STATE_AUTO_IP:
                            ipcfg_bind_autoip_internal (i, NULL);
                            break;
                        case IPCFG_STATE_DHCPAUTO_IP:
                            error = ipcfg_bind_dhcp_internal (i, TRUE);
                            if ((error == IPCFG_OK) || (error == RTCSERR_IPCFG_BUSY))
                            {
                                ipcfg_poll_dhcp_internal (i, TRUE, NULL);
                            }
                            break;
                        case IPCFG_STATE_BOOT:
                            ipcfg_bind_boot_internal (i);
                            break;
                        default: 
                            break;                      
                    }
                } 
				else if ((! link) && (actual_state != IPCFG_STATE_UNBOUND)) 
				{
					// link is down, unbind any interface
					ipcfg_unbind_internal (i);
				}

                // if busy, don't finish the task and check it next time
                can_finish &= (actual_state != IPCFG_STATE_BUSY);

                _lwsem_post (&(ipcfg_data[i].control_semaphore));
                
            }
            else
            {
                // interface was busy, don't finish the task and check it next time
                actual_state = IPCFG_STATE_BUSY;
                can_finish = FALSE;
            }

#if IPCFG_DEBUG_LEVEL >= 2
            printf ("Eth#:%d link:%s state:%s\n", i, link ? "on" : "off", ipcfg_get_state_string (actual_state));
#endif
        }
    }

    return can_finish;
}


/*TASK*--------------------------------------------------------------
*
* Function Name  : ipcfg_monitor_task()
* Returned Value : void
* Comments       : All ethernet devices monitoring/binding task.
*
*END----------------------------------------------------------------*/

static void ipcfg_monitor_task 
    (
        uint_32     check_period_ms
    )
{
    boolean can_finish;

#if IPCFG_DEBUG_LEVEL >= 2
    printf ("IPCFG task created.\n");
#endif
    
    while (TRUE) 
    {
        can_finish = ipcfg_task_poll ();

        if ((! ipcfg_task_run) && can_finish) break;

        _time_delay (check_period_ms);
    }

#if IPCFG_DEBUG_LEVEL >= 2
    printf ("IPCFG task destroyed.\n");
#endif
    _task_destroy (MQX_NULL_TASK_ID);
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_task_create()
* Returned Value : MQX error code
* Comments       : Starts ipcfg task, if not started yet.
*
*END--------------------------------------------------------------------*/

uint_32 ipcfg_task_create
    (
        uint_32 priority, 
        uint_32 task_period_ms
    )
{
    TASK_TEMPLATE_STRUCT  task_templ;
    _task_id              tid;
	uint_32               tec;
    uint_32               result;

    if (ipcfg_task_status ())
    {
        return MQX_DUPLICATE_TASK_TEMPLATE_INDEX;
    }

    ipcfg_task_run = TRUE;

	tec = _task_get_error();
    _mem_zero ((uchar_ptr)&task_templ, sizeof (task_templ));
    task_templ.TASK_ADDRESS       = ipcfg_monitor_task;
    task_templ.TASK_STACKSIZE     = IPCFG_TASK_STACK_SIZE;
    task_templ.TASK_PRIORITY      = priority;
    task_templ.TASK_NAME          = (char_ptr)ipcfg_task_name;
    task_templ.CREATION_PARAMETER = task_period_ms;
    
    tid = _task_create (0, 0, (uint_32)&task_templ);
    if (tid == MQX_NULL_TASK_ID) 
    {
        result = MQX_INVALID_TASK_ID;
    } 
    else 
    {
        result = MQX_OK;
    }
	_task_set_error(tec);

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_task_destroy()
* Returned Value : void
* Comments       : Sets flag to destroy ipcfg task, can wait for completion.
*
*END--------------------------------------------------------------------*/

void ipcfg_task_destroy
    (
        boolean wait_task_finish
    )
{
    _task_id    tid;
    uint_32     period;

    ipcfg_task_run = FALSE;
    if (! wait_task_finish) return;
    do
    {
        tid = _task_get_id_from_name ((char_ptr)&ipcfg_task_name);
        if (tid != MQX_NULL_TASK_ID) 
        {
            period = _task_get_parameter_for (tid);
            if ((period < 10 || period > 10000)) period = 1000;
            _time_delay (period);
        }
    } while (tid != MQX_NULL_TASK_ID);
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_task_status()
* Returned Value : TRUE if running, FALSE otherwise
* Comments       : Checks whether task is running.
*
*END--------------------------------------------------------------------*/

boolean ipcfg_task_status (void)
{
    _task_id tid;
    
    tid = _task_get_id_from_name ((char_ptr)&ipcfg_task_name);
    if (tid != MQX_NULL_TASK_ID) 
    {
        return TRUE;
    }
    return FALSE;   
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_init_device()
* Returned Value : error code
* Comments       : Initializes given enet device and associated ip interface.
*
*END--------------------------------------------------------------------*/

uint_32 ipcfg_init_device
    (
        uint_32         device,
        _enet_address   mac
    )
{
    _enet_handle    ehandle;
    _rtcs_if_handle ihandle;
    uint_32         error = 0;
    
    error = ENET_initialize (device, mac, 0, &ehandle);
    
    if (error == RTCS_OK) 
    {
        error = RTCS_if_add (ehandle, RTCS_IF_ENET, &ihandle);
        if (error == RTCS_OK) 
        {
            error = ipcfg_init_interface(device, ihandle);
            if (error != RTCS_OK)
            {
                RTCS_if_remove (ihandle);
                ENET_shutdown (ehandle);
#if IPCFG_DEBUG_LEVEL >= 2
                printf ("IPCFG init error: %08x\n", error);
#endif
            }
        }
        else 
        {
            ENET_shutdown (ehandle);
#if IPCFG_DEBUG_LEVEL >= 2
            printf ("Interface add error: %08x\n", error);
#endif
        }
        
    }
    else if (error == ENETERR_INIT_DEVICE)
    {
#if IPCFG_DEBUG_LEVEL >= 2
        printf ("Device reinitialization... \n");
#endif
    }
    else  
    {
#if IPCFG_DEBUG_LEVEL >= 2
        printf ("Device initialization error: %08x\n", error);
#endif
    }
    
    return error;
}

/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_release_device()
* Returned Value : error code
* Comments       : Releases given enet device and associated ip interface.
*
*END--------------------------------------------------------------------*/
uint_32 ipcfg_release_device(uint_32  device )
{
    _enet_handle    ehandle;
    _rtcs_if_handle ihandle;
    uint_32         error = 0;

    if (device < IPCFG_DEVICE_COUNT)
    {
        ihandle = ipcfg_get_ihandle(device);
        if(ihandle != NULL)
        {
            ehandle = RTCS_get_enet_handle(ihandle);

            error = ipcfg_release_interface(device);
            if (error == RTCS_OK)
            {
                error = RTCS_if_remove (ihandle);
                if (error == RTCS_OK)
                {
                    error = ENET_shutdown (ehandle);
                }
            }
            return error;
        }
        else /* Not initialised by ipcfg_init_device().*/
            return RTCSERR_IPCFG_NOT_INIT;

    }
    else
        return RTCSERR_IPCFG_DEVICE_NUMBER;

}

/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_init_interface()
* Returned Value : ipcfg error code
* Comments       : Initializes interface related internal structure - once.
*
*END--------------------------------------------------------------------*/

uint_32 ipcfg_init_interface 
    (
        uint_32 dev_num,
        _rtcs_if_handle ihandle
    )
{
    _mqx_uint device;

    if (ihandle == NULL) return RTCSERR_IPCFG_INIT; 
    
    device = ipcfg_add_interface(dev_num, ihandle);
    
    if (device == (uint_32)-1)
        return RTCSERR_IPCFG_INIT;
    
    if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT)
        return RTCSERR_IPCFG_BUSY;

    if (_lwsem_create (&ipcfg_data[device].control_semaphore, 1) != MQX_OK)
    {
        return RTCSERR_IPCFG_INIT;
    }
    if (_lwsem_create (&ipcfg_data[device].request_semaphore, 1) != MQX_OK)
    {
        _lwsem_destroy (&ipcfg_data[device].control_semaphore);
        return RTCSERR_IPCFG_INIT;
    }
#if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
    if (_lwsem_create (&ipcfg_data[device].dhcp_response_semaphore, 0) != MQX_OK)
    {
        _lwsem_destroy (&ipcfg_data[device].control_semaphore);
        _lwsem_destroy (&ipcfg_data[device].request_semaphore);
        return RTCSERR_IPCFG_INIT;
    }
    ipcfg_data[device].dhcp_retries = 0;
#endif  
    ipcfg_data[device].ihandle = ihandle;
    ipcfg_data[device].desired_ip_data.ip = 0;
    ipcfg_data[device].desired_ip_data.mask = 0;
    ipcfg_data[device].desired_ip_data.gateway = 0;
    ipcfg_data[device].actual_ip_data.ip = 0;
    ipcfg_data[device].actual_ip_data.mask = 0;
    ipcfg_data[device].actual_ip_data.gateway = 0;
#if RTCSCFG_IPCFG_ENABLE_DNS
    ipcfg_data[device].dns = 0;
#endif
#if RTCSCFG_IPCFG_ENABLE_BOOT   
    ipcfg_data[device].tftp_serveraddress = 0;
    ipcfg_data[device].tftp_servername[0] = ipcfg_empty_string[0];
    ipcfg_data[device].boot_filename[0] = ipcfg_empty_string[0];
#endif  
    ipcfg_data[device].desired_state = IPCFG_STATE_UNBOUND;
    ipcfg_data[device].actual_state = IPCFG_STATE_UNBOUND;
    
    return RTCS_OK;    
}

/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : ipcfg_release_interface()
* Returned Value : ipcfg error code
* Comments       : Releases interface related internal structure.
*
*END--------------------------------------------------------------------*/
uint_32 ipcfg_release_interface ( uint_32 dev_num)
{
    if (dev_num < IPCFG_DEVICE_COUNT)
    {
        ipcfg_unbind (dev_num);

    #if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
        _lwsem_destroy (&ipcfg_data[dev_num].dhcp_response_semaphore);
    #endif
        _lwsem_destroy (&ipcfg_data[dev_num].request_semaphore);
        _lwsem_destroy (&ipcfg_data[dev_num].control_semaphore);

        ipcfg_del_interface(dev_num);
        return RTCS_OK;
    }
    else
        return RTCSERR_IPCFG_DEVICE_NUMBER;
}



#endif /* (BSP_ENET_DEVICE_COUNT > 0) */
#endif /* BSP_ENET_DEVICE_COUNT */
