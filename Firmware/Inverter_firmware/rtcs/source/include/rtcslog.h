#ifndef __rtcslog_h__
#define __rtcslog_h__
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
* $FileName: rtcslog.h$
* $Version : 3.8.9.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the definitions for RTCS logging.
*
*END************************************************************************/

#include "mqx_cnfg.h"
#include "rtcscfg.h"

/*
** Code macros
*/

//#define RTCSLOG_NONE(t,p1,p2,p3,p4,p5)

#if RTCSCFG_LOGGING
#define RTCSLOG_LOG(t,p1,p2,p3,p4,p5) RTCS_log(t,p1,p2,p3,p4,p5)
#else
#define RTCSLOG_LOG(t,p1,p2,p3,p4,p5)
#endif 

#if RTCSCFG_LOG_SOCKET_API
#define RTCSLOG_API(t,p1,p2)     RTCS_log(t,(uint_32)(p1),(uint_32)(p2),0,0,0)
#else
#define RTCSLOG_API(t,p1,p2)      
#endif

#if RTCSCFG_LOG_PCB
#define RTCSLOG_PCB(t,p1,p2)     RTCS_log(t,p1,p2,0,0,0)
#else
#define RTCSLOG_PCB(t,p1,p2)     
#endif

#define RTCSLOG_PCB_ALLOC(pcb)            RTCSLOG_PCB( RTCSLOG_TYPE_PCB, \
                                                       RTCSLOG_PCBCMD_ALLOC, \
                                                       (uint_32)pcb)

#define RTCSLOG_PCB_FREE(pcb,err)         RTCSLOG_PCB( RTCSLOG_TYPE_PCB, \
                                                       RTCSLOG_PCBCMD_FREE | (err), \
                                                       (uint_32)pcb)

#define RTCSLOG_PCB_READ(pcb,prot,vers)   RTCSLOG_PCB( RTCSLOG_TYPE_PCB, \
                                                       RTCSLOG_PCBCMD_READ | ((vers) << 16) | (prot), \
                                                       (uint_32)pcb)

#define RTCSLOG_PCB_WRITE(pcb,prot,vers)  RTCSLOG_PCB( RTCSLOG_TYPE_PCB, \
                                                       RTCSLOG_PCBCMD_WRITE | ((vers) << 16) | (prot), \
                                                       (uint_32)pcb)

#define RTCSLOG_TIMER   RTCSLOG_NONE

#define RTCSLOG_FNE1(f)             RTCSLOG_LOG(RTCSLOG_TYPE_FNENTRY, f, 0, 0, 0, 0)
#define RTCSLOG_FNE2(f, a)          RTCSLOG_LOG(RTCSLOG_TYPE_FNENTRY, f, (uint_32)(a), 0, 0, 0)
#define RTCSLOG_FNE3(f, a, b)       RTCSLOG_LOG(RTCSLOG_TYPE_FNENTRY, f, (uint_32)(a), (uint_32)(b), 0, 0)
#define RTCSLOG_FNE4(f, a, b, c)    RTCSLOG_LOG(RTCSLOG_TYPE_FNENTRY, f, (uint_32)(a), (uint_32)(b), (uint_32)(c), 0)
#define RTCSLOG_FNE5(f, a, b, c, d) RTCSLOG_LOG(RTCSLOG_TYPE_FNENTRY, f, (uint_32)(a), (uint_32)(b), (uint_32)(c), (uint_32)(d))

#define RTCSLOG_FNX1(f)             RTCSLOG_LOG(RTCSLOG_TYPE_FNEXIT, f, 0, 0, 0, 0)
#define RTCSLOG_FNX2(f, a)          RTCSLOG_LOG(RTCSLOG_TYPE_FNEXIT, f, (uint_32)(a), 0, 0, 0)
#define RTCSLOG_FNX3(f, a , b)      RTCSLOG_LOG(RTCSLOG_TYPE_FNEXIT, f, (uint_32)(a), (uint_32)(b), 0, 0)
#define RTCSLOG_FNX4(f, a , b, c)   RTCSLOG_LOG(RTCSLOG_TYPE_FNEXIT, f, (uint_32)(a), (uint_32)(b), (uint_32)(c), 0)

/*
** Obsolete macros
*/

#define DEBUGTCP(x)     /*x*/

/*
** PCB actions
*/

#define RTCSLOG_PCBCMD_ALLOC     0x00000000
#define RTCSLOG_PCBCMD_FREE      0x40000000
#define RTCSLOG_PCBCMD_READ      0x80000000
#define RTCSLOG_PCBCMD_WRITE     0xC0000000
#define RTCSLOG_PCBCMD_MASK      0xC0000000


/*
** Function numbers
*/
#define RTCSLOG_FNBASE                 (KLOG_RTCS_FUNCTIONS)

#define RTCSLOG_FN_SOCKET              (RTCSLOG_FNBASE   )
#define RTCSLOG_FN_DGRAM_BIND          (RTCSLOG_FNBASE+ 1)
#define RTCSLOG_FN_DGRAM_CONNECT       (RTCSLOG_FNBASE+ 2)
#define RTCSLOG_FN_DGRAM_LISTEN        (RTCSLOG_FNBASE+ 3)
#define RTCSLOG_FN_DGRAM_ACCEPT        (RTCSLOG_FNBASE+ 4)
#define RTCSLOG_FN_DGRAM_GETSOCKNAME   (RTCSLOG_FNBASE+ 5)
#define RTCSLOG_FN_DGRAM_GETPEERNAME   (RTCSLOG_FNBASE+ 6)
#define RTCSLOG_FN_DGRAM_RECV          (RTCSLOG_FNBASE+ 7)
#define RTCSLOG_FN_DGRAM_RECVFROM      (RTCSLOG_FNBASE+ 8)
#define RTCSLOG_FN_DGRAM_RECVMSG       (RTCSLOG_FNBASE+ 9)
#define RTCSLOG_FN_DGRAM_SEND          (RTCSLOG_FNBASE+10)
#define RTCSLOG_FN_DGRAM_SENDTO        (RTCSLOG_FNBASE+11)
#define RTCSLOG_FN_DGRAM_SENDMSG       (RTCSLOG_FNBASE+12)
#define RTCSLOG_FN_DGRAM_SOCKATMARK    (RTCSLOG_FNBASE+13)
#define RTCSLOG_FN_DGRAM_SHUTDOWN      (RTCSLOG_FNBASE+14)
#define RTCSLOG_FN_STREAM_BIND         (RTCSLOG_FNBASE+15)
#define RTCSLOG_FN_STREAM_CONNECT      (RTCSLOG_FNBASE+16)
#define RTCSLOG_FN_STREAM_LISTEN       (RTCSLOG_FNBASE+17)
#define RTCSLOG_FN_STREAM_ACCEPT       (RTCSLOG_FNBASE+18)
#define RTCSLOG_FN_STREAM_GETSOCKNAME  (RTCSLOG_FNBASE+19)
#define RTCSLOG_FN_STREAM_GETPEERNAME  (RTCSLOG_FNBASE+20)
#define RTCSLOG_FN_STREAM_RECV         (RTCSLOG_FNBASE+21)
#define RTCSLOG_FN_STREAM_RECVFROM     (RTCSLOG_FNBASE+22)
#define RTCSLOG_FN_STREAM_RECVMSG      (RTCSLOG_FNBASE+23)
#define RTCSLOG_FN_STREAM_SEND         (RTCSLOG_FNBASE+24)
#define RTCSLOG_FN_STREAM_SENDTO       (RTCSLOG_FNBASE+25)
#define RTCSLOG_FN_STREAM_SENDMSG      (RTCSLOG_FNBASE+26)
#define RTCSLOG_FN_STREAM_SOCKATMARK   (RTCSLOG_FNBASE+27)
#define RTCSLOG_FN_STREAM_SHUTDOWN     (RTCSLOG_FNBASE+28)
#define RTCSLOG_FN_SOCKET_GETSOCKOPT   (RTCSLOG_FNBASE+29)
#define RTCSLOG_FN_SOCKET_SETSOCKOPT   (RTCSLOG_FNBASE+30)
#define RTCSLOG_FN_UDP_GETSOCKOPT      (RTCSLOG_FNBASE+31)
#define RTCSLOG_FN_UDP_SETSOCKOPT      (RTCSLOG_FNBASE+32)
#define RTCSLOG_FN_TCP_GETSOCKOPT      (RTCSLOG_FNBASE+33)
#define RTCSLOG_FN_TCP_SETSOCKOPT      (RTCSLOG_FNBASE+34)
#define RTCSLOG_FN_IP_GETSOCKOPT       (RTCSLOG_FNBASE+35)
#define RTCSLOG_FN_IP_SETSOCKOPT       (RTCSLOG_FNBASE+36)
#define RTCSLOG_FN_IGMP_GETSOCKOPT     (RTCSLOG_FNBASE+37)
#define RTCSLOG_FN_IGMP_SETSOCKOPT     (RTCSLOG_FNBASE+38)
#define RTCSLOG_FN_LINK_GETSOCKOPT     (RTCSLOG_FNBASE+39)
#define RTCSLOG_FN_LINK_SETSOCKOPT     (RTCSLOG_FNBASE+40)
#define RTCSLOG_FN_NAT_GETSOCKOPT      (RTCSLOG_FNBASE+41)
#define RTCSLOG_FN_NAT_SETSOCKOPT      (RTCSLOG_FNBASE+42)

#define RTCSLOG_FN_TCP_Must_send_ack                (RTCSLOG_FNBASE + 43)
#define RTCSLOG_FN_TCP_Process_data                 (RTCSLOG_FNBASE + 44)
#define RTCSLOG_FN_TCP_Advertise_receive_window     (RTCSLOG_FNBASE + 45)
#define RTCSLOG_FN_TCP_Start_receiving              (RTCSLOG_FNBASE + 46)
#define RTCSLOG_FN_TCP_Process_packet               (RTCSLOG_FNBASE + 47)
#define RTCSLOG_FN_TCP_Process_effective_close      (RTCSLOG_FNBASE + 48)
#define RTCSLOG_FN_TCP_Service_packet               (RTCSLOG_FNBASE + 49)
#define RTCSLOG_FN_IPLOCAL_service                  (RTCSLOG_FNBASE + 50)
#define RTCSLOG_FN_IPLOCAL_send                     (RTCSLOG_FNBASE + 51)
#define RTCSLOG_FN_IP_send_dgram                    (RTCSLOG_FNBASE + 52)
#define RTCSLOG_FN_IP_route                         (RTCSLOG_FNBASE + 53)
#define RTCSLOG_FN_IP_service                       (RTCSLOG_FNBASE + 54)
#define RTCSLOG_FN_TCPIP_task                       (RTCSLOG_FNBASE + 55)
#define RTCSLOG_FN_TCP_Event                        (RTCSLOG_FNBASE + 56)
#define RTCSLOG_FN_TCP_Process_signal               (RTCSLOG_FNBASE + 57)
#define RTCSLOG_FN_TCP_Tick_server                  (RTCSLOG_FNBASE + 58)
#define RTCSLOG_FN_TCP_Timer_expire                 (RTCSLOG_FNBASE + 59)
#define RTCSLOG_FN_TCP_Timer_schedule               (RTCSLOG_FNBASE + 60)
#define RTCSLOG_FN_TCP_Timer_remove                 (RTCSLOG_FNBASE + 61)
#define RTCSLOG_FN_TCP_Timer_stop                   (RTCSLOG_FNBASE + 62)
#define RTCSLOG_FN_TCP_Timer_start                  (RTCSLOG_FNBASE + 63)
#define RTCSLOG_FN_TCP_Timer_advance                (RTCSLOG_FNBASE + 64)
#define RTCSLOG_FN_TCP_Timer_oneshot_max            (RTCSLOG_FNBASE + 65)

#define RTCSLOG_FN_IPE_recv                         (RTCSLOG_FNBASE + 66)
#define RTCSLOG_FN_IPE_recv_IP                      (RTCSLOG_FNBASE + 67)
#define RTCSLOG_FN_IPE_send                         (RTCSLOG_FNBASE + 68)
#define RTCSLOG_FN_IPE_send_IP                      (RTCSLOG_FNBASE + 69)
#define RTCSLOG_FN_TCP_Transmit                     (RTCSLOG_FNBASE + 70)
#define RTCSLOG_FN_TCP_Send_packet                  (RTCSLOG_FNBASE + 71)

#define RTCSLOG_FN_IP6E_recv                        (RTCSLOG_FNBASE + 72)
#define RTCSLOG_FN_IP6E_recv_IP                     (RTCSLOG_FNBASE + 73)
#define RTCSLOG_FN_IP6_service                      (RTCSLOG_FNBASE + 74)
#define RTCSLOG_FN_IP6_send_dgram                   (RTCSLOG_FNBASE + 75)    


/*
** Prototypes
*/

#ifdef __cplusplus
extern "C" {
#endif

#if RTCSCFG_LOGGING
void RTCS_log (uint_32 logtype, uint_32 p1, uint_32 p2,uint_32 p3, uint_32 p4, uint_32 p5);

void RTCSLOG_enable       (uint_32 logtype);
void RTCSLOG_disable      (uint_32 logtype);
void RTCSLOG_enable_prot  (uint_32 logtype, uint_32 protocol);
void RTCSLOG_disable_prot (uint_32 logtype, uint_32 protocol);
#else
#define RTCS_log(t,p1,p2,p3,p4,p5)
#define RTCSLOG_enable(x)
#define RTCSLOG_disable(x)
#define RTCSLOG_enable_prot(t, x)
#define RTCSLOG_disable_prot(t, x)
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
