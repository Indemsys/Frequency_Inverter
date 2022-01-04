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
* $FileName: checksum6.c$
* $Version : 3.8.1.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation for a one's
*   complement checksum (IPv6 version).
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP6

#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"


/************************************************************************
* NAME: IP6_Sum_pseudo
*
* DESCRIPTION: Returns a one's complement checksum of the IP pseudo header
*
*     Note:  This function returns 0 iff all summands are 0..
*************************************************************************/
uint_16 IP6_Sum_pseudo( uint_16 sum /* [IN] initial sum */, RTCSPCB_PTR pcb /* [IN] the PCB */)
{
    IP6_HEADER_PTR iphead = (IP6_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb);
    uint_16 protocol;

    protocol = RTCSPCB_TRANSPORT_PROTL(pcb);
    sum = _mem_sum_ip(sum, 32, iphead->SOURCE); /* source and dest */
    sum = IP_Sum_immediate(sum, protocol);      /* protocol */
    return sum;
}

#endif /* RTCSCFG_ENABLE_IP6 */
