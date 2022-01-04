#ifndef __ipcfg_prv_h__
#define __ipcfg_prv_h__
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
* $FileName: ipcfg_prv.h$
* $Version : 3.8.4.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Private definitions for the IPCFG layer.
*
*END************************************************************************/


#include <rtcs.h>


typedef struct ipcfg_context
{
    /* runtime context */
    LWSEM_STRUCT                control_semaphore;
    LWSEM_STRUCT                request_semaphore;
#if RTCSCFG_IPCFG_ENABLE_DHCP && RTCSCFG_ENABLE_IP4
    LWSEM_STRUCT                dhcp_response_semaphore;
    uint_32                     dhcp_retries;
#endif
    _rtcs_if_handle             ihandle;
    
    /* parameters for monitoring task */
    IPCFG_STATE                 desired_state;
    IPCFG_IP_ADDRESS_DATA       desired_ip_data;
    
    /* results */ 
    IPCFG_STATE                 actual_state;
    IPCFG_IP_ADDRESS_DATA       actual_ip_data;
#if RTCSCFG_IPCFG_ENABLE_DNS
    _ip_address                 dns;
#endif
#if RTCSCFG_IPCFG_ENABLE_BOOT
    _ip_address                 tftp_serveraddress;
    uchar                       tftp_servername[RTCS_SNAME_LEN];
    uchar                       boot_filename[RTCS_BOOTFILE_LEN];
#endif  
} IPCFG_CONTEXT, _PTR_ IPCFG_CONTEXT_PTR;


#endif
/* EOF */
