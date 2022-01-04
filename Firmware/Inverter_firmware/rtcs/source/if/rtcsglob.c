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
* $FileName: rtcsglob.c$
* $Version : 3.4.8.0$
* $Date    : Nov-19-2009$
*
* Comments:
*
*   Definitions of global variables required for RTCS.
*
*END************************************************************************/

#include <rtcs.h>


uint_32  _RTCSQUEUE_base = RTCSCFG_QUEUE_BASE;

/*
** Number of RTCSPCBs to allocate
*/
uint_32  _RTCSPCB_init = RTCSCFG_PCBS_INIT;
uint_32  _RTCSPCB_grow = RTCSCFG_PCBS_GROW;
uint_32  _RTCSPCB_max  = RTCSCFG_PCBS_MAX;

/*
** TCP/IP task priority and extra stack size
*/
uint_32  _RTCSTASK_priority   = 6;
uint_32  _RTCSTASK_stacksize  = RTCSCFG_STACK_SIZE;
boolean  _RTCS_initialized    = FALSE;

/*
** TCP-based RPC task priority and extra stack size
*/
uint_32  _SVCTCPTASK_priority    = 9;
uint_32  _SVCTCPTASK_stacksize   = 0;

/*
** Indicates whether or not IP should forward packets between interfaces
*/
boolean  _IP_forward = FALSE;

/*
** Indicates whether or not to bypass checksum checking
*/
boolean  _TCP_bypass_rx = FALSE;
boolean  _TCP_bypass_tx = FALSE;

/*
** The TCP retransmission timeout minimum
*/
uint_32  _TCP_rto_min;     /* initialized by TCP_Init() */

/*
** Maximum number of packets that will be enqueued
*/
uint_32  _UDP_max_queue_size  = RTCSCFG_UDP_MAX_QUEUE_SIZE;

/* 
** Use the dhcp broadcast flag, or dhcp unicast offers. 
*/
boolean _DHCP_broadcast = FALSE;

/*
** Pool that RTCS should allocate memory from. If 0, susyem pool will be used
*/
_mem_pool_id _RTCS_mem_pool = 0;


uint_32 _RTCS_msgpool_init  = RTCSCFG_MSGPOOL_INIT;
uint_32 _RTCS_msgpool_grow  = RTCSCFG_MSGPOOL_GROW;
uint_32 _RTCS_msgpool_max   = RTCSCFG_MSGPOOL_MAX;


uint_32 _RTCS_socket_part_init  = RTCSCFG_SOCKET_PART_INIT;
uint_32 _RTCS_socket_part_grow  = RTCSCFG_SOCKET_PART_GROW;
uint_32 _RTCS_socket_part_max   = RTCSCFG_SOCKET_PART_MAX;

/* 
** Use following dhcp termination timeout. 
*/
uint_32 _RTCS_dhcp_term_timeout = 60;

/* EOF */
