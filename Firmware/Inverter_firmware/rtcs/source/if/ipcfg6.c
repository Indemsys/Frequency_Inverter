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
* $FileName: ipcfg6.c$
* $Version : 3.8.2.0$
* $Date    : Jun-27-2012$
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

/* Hidden global data. */
extern IPCFG_CONTEXT    ipcfg_data[IPCFG_DEVICE_COUNT];

/************************************************************************
* NAME: ipcfg6_get_addr_info_n
* RETURNS : RTCS_OK if successful. 
*           It returns FALSE if n-th address is not available.
* DESCRIPTION: Gets address information which corresponds to the n-th address.
*************************************************************************/
uint_32 ipcfg6_get_addr_info_n (uint_32 device, uint_32 n /*=>0*/, IPCFG6_GET_ADDR_DATA_PTR data /*out*/)
{
#if RTCSCFG_ENABLE_IP6

    uint_32 result = (uint_32)RTCS_ERROR;
    
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) 
        {
            IP6_IF_ADDR_INFO addr_info;
            
            /* Get address inormation.*/
            if(ip6_if_get_addr (ipcfg_data[device].ihandle, n, &addr_info) == TRUE)
            {
                data->ip_addr_type = addr_info.ip_addr_type;
                data->ip_addr_state = addr_info.ip_addr_state;
                _mem_copy(addr_info.ip_addr.s6_addr, data->ip_addr.s6_addr, sizeof(data->ip_addr));
                result = RTCS_OK;
            }
        }
    }
    return result;
    
#else

    return RTCSERR_IP_IS_DISABLED;    
    
#endif /* RTCSCFG_ENABLE_IP6 */
}

/************************************************************************
* NAME: ipcfg6_get_device_scope_id
* RETURNS : Scope id of device.
*************************************************************************/
uint_32 ipcfg6_get_device_scope_id (uint_32 device /* in */)
{
#if RTCSCFG_ENABLE_IP6

    uint_32 result = (uint_32)RTCS_ERROR;
    
    if (device < IPCFG_DEVICE_COUNT)
    {
        if (ipcfg_data[device].actual_state != IPCFG_STATE_INIT) 
        {
            IP_IF_PTR if_ptr = (IP_IF_PTR)(ipcfg_data[device].ihandle);
            
            /* Get scope_id inormation.*/

           return (if_ptr->IP6_IF.scope_id);
        }
    }
    return 0;
    
#else

    return 0;    
    
#endif /* RTCSCFG_ENABLE_IP6 */
}

/************************************************************************
* NAME: ipcfg6_get_addr_info
* RETURNS : TRUE if successful.
* DESCRIPTION: Gets address information which corresponds to the "addr".
*************************************************************************/
uint_32 ipcfg6_get_addr_info (uint_32 device,
                             in6_addr *addr /*in*/,  
                             IPCFG6_GET_ADDR_DATA_PTR data/*out*/)
{
#if RTCSCFG_ENABLE_IP6

    uint_32 result = (uint_32)RTCS_ERROR;
    IP6_IF_ADDR_INFO_PTR addr_info;
    
    addr_info = ip6_if_get_addr_info((IP_IF_PTR) ipcfg_data[device].ihandle, addr);
    
    if(addr_info)
    {
        data->ip_addr_type = addr_info->ip_addr_type;
        data->ip_addr_state = addr_info->ip_addr_state;
        _mem_copy(addr_info->ip_addr.s6_addr, data->ip_addr.s6_addr, sizeof(data->ip_addr));
        result = RTCS_OK;
    }
    
    return result;
    
#else

    return RTCSERR_IP_IS_DISABLED;    
    
#endif /* RTCSCFG_ENABLE_IP6 */    
}

/************************************************************************
* NAME: ipcfg6_unbind_addr
* RETURNS : ipcfg error code
* DESCRIPTION: Releases an IP address associated with given device.
*************************************************************************/
uint_32 ipcfg6_unbind_addr ( uint_32 device, IPCFG6_UNBIND_ADDR_DATA_PTR  ip_data)
{
#if RTCSCFG_ENABLE_IP6
 
    uint_32 result;

    if (device >= IPCFG_DEVICE_COUNT) return RTCSERR_IPCFG_DEVICE_NUMBER;

    if (ipcfg_data[device].actual_state == IPCFG_STATE_INIT) 
        return RTCSERR_IPCFG_INIT;
    
    if (! _lwsem_poll (&(ipcfg_data[device].control_semaphore))) 
        return RTCSERR_IPCFG_BUSY;
    
    /* Unbind.*/
    result = RTCS6_if_unbind_addr (ipcfg_data[device].ihandle, ip_data->ip_addr);

    if (result == IPCFG_OK)
    {
        ipcfg_data[device].desired_ip_data = ipcfg_data[device].actual_ip_data;
        ipcfg_data[device].desired_state = ipcfg_data[device].actual_state;
    }

    _lwsem_post (&(ipcfg_data[device].control_semaphore));

    return result; 
     
#else

    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP6 */
    
}

/************************************************************************
* NAME: ipcfg6_bind_addr
* RETURNS : ipcfg error code
* DESCRIPTION: Tries to bind given ip for given device.
*************************************************************************/
uint_32 ipcfg6_bind_addr ( uint_32 device, IPCFG6_BIND_ADDR_DATA_PTR   ip_data)
{
#if RTCSCFG_ENABLE_IP6

    uint_32 result;

    if (device >= IPCFG_DEVICE_COUNT) 
        return RTCSERR_IPCFG_DEVICE_NUMBER;

    if (ipcfg_data[device].actual_state == IPCFG_STATE_INIT) 
        return RTCSERR_IPCFG_INIT;

    if (! _lwsem_poll (&(ipcfg_data[device].control_semaphore))) 
        return RTCSERR_IPCFG_BUSY;
    
    if (! _lwsem_poll (&(ipcfg_data[device].request_semaphore))) return RTCSERR_IPCFG_BUSY;

    result = RTCSERR_IPCFG_BIND;
    
    /* Bind.*/
    if (RTCS6_if_bind_addr (ipcfg_data[device].ihandle, ip_data->ip_addr, ip_data->ip_addr_type) == RTCS_OK)
    {
        ipcfg_data[device].desired_state = IPCFG_STATE_STATIC_IP;
        ipcfg_data[device].actual_state = ipcfg_data[device].desired_state;
        result = IPCFG_OK;
    }

    _lwsem_post (&(ipcfg_data[device].request_semaphore));    
    

    _lwsem_post (&(ipcfg_data[device].control_semaphore));
    
    return result;
#else

    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP6 */    
}


#endif /* (BSP_ENET_DEVICE_COUNT > 0) */
#endif /* BSP_ENET_DEVICE_COUNT */
