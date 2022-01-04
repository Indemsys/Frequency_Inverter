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
* $FileName: rtcs_prv.h$
* $Version : 3.8.13.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the include files for the
*   RTCS Communication Library.
*
*END************************************************************************/

#ifndef __rtcs_prv_h__
#define __rtcs_prv_h__

#include "rtcspcb.h"

/*
** RTCS queue numbers
*/
#define TCPIP_QUEUE     _RTCSQUEUE_base


/*
** Maximum number of protocols that can be logged
*/
#define RTCSLOG_PROT_MAX   16


/*
** Macro definitions for manipulating common data structures
*/

   /*
   ** RTCS_QUEUE: a singly-linked FIFO list
   **
   ** - requires a head and a tail
   ** - each element requires a field called NEXT
   */

#define RTCS_QUEUE_INIT(head,tail)              \
      (head) = NULL

#define RTCS_QUEUE_PEEK(head,tail,elem)         \
      (elem) = (head)

#define RTCS_QUEUE_INS(head,tail,elem)          \
      (elem)->NEXT = NULL;                      \
      if (head) {                               \
         (tail)->NEXT = (elem);                 \
         (tail) = (elem);                       \
      } else {                                  \
         (head) = (elem);                       \
         (tail) = (elem);                       \
      }

#define RTCS_QUEUE_DEL(head,tail)               \
      if (head) {                               \
         (head) = (head)->NEXT;                 \
      }

#define RTCS_QUEUE_DEL_NONEMPTY(head,tail)      \
      (head) = (head)->NEXT

#define RTCS_QUEUE_DELALL(head,tail,elem,temp)  \
      for ((elem)=(head);                       \
           (elem)?(temp)=(elem)->NEXT:0,        \
           (elem);                              \
           (elem)=(temp))

   /*
   ** RTCS_LIST: a singly-linked list
   **
   ** - requires a head
   ** - each element requires a field called NEXT
   */

#define RTCS_LIST_INIT(head)                    \
      (head) = NULL

#define RTCS_LIST_INS(head,elem)                \
      (elem)->NEXT = (head);                    \
      (head) = (elem)

#define RTCS_LIST_INS_END(head,elem,tempp)      \
      for ((tempp)=&(head);                     \
            *(tempp);                           \
            (tempp)=&(*(tempp))->NEXT) ;        \
      *(tempp) = (elem);                        \
      (elem)->NEXT = NULL

#define RTCS_LIST_PEEK(head,elem)               \
      (elem) = (head)

#define RTCS_LIST_SEARCH(head,elem)             \
      for ((elem)=(head);                       \
           (elem);                              \
           (elem)=(elem)->NEXT)

#define RTCS_LIST_DEL(head,elemp)               \
      *(elemp) = (*(elemp))->NEXT

#define RTCS_LIST_SEARCHMOD(head,elemp)         \
      for ((elemp)=&(head);                     \
           *(elemp);                            \
           (elemp)=&(*(elemp))->NEXT)

#define RTCS_LIST_SEARCHMOD_NOHEAD(head,elemp)  \
      for ((elemp)=&(head)->NEXT;               \
           *(elemp);                            \
           (elemp)=&(*(elemp))->NEXT)

   /*
   ** RTCS_DLIST: a doubly-linked list
   **
   ** - requires a head
   ** - each element requires fields called NEXT and PREV
   */

#define RTCS_DLIST_INIT(head)                   \
      (head) = NULL

#define RTCS_DLIST_INS(head,elem)               \
      (elem)->NEXT = (head);                    \
      (elem)->PREV = &(head);                   \
      if (head) {                               \
         (head)->PREV = &(elem)->NEXT;          \
      }                                         \
      (head) = (elem)

#define RTCS_DLIST_PEEK(head,elem)              \
      (elem) = (head)

#define RTCS_DLIST_SEARCH(head,elem)            \
      for ((elem)=(head);                       \
           (elem);                              \
           (elem)=(elem)->NEXT)

#define RTCS_DLIST_SEARCH_REST(head,elem)       \
      for ((elem)=(elem)->NEXT;                 \
           (elem);                              \
           (elem)=(elem)->NEXT)

#define RTCS_DLIST_DEL(head,elem)               \
      if ((elem)->NEXT) {                       \
         (elem)->NEXT->PREV = (elem)->PREV;     \
      }                                         \
      *(elem)->PREV = (elem)->NEXT


/*
** Macro definitions for RTCS
*/

#define RTCSCMD_issue(parm,cmd)      RTCS_cmd_issue(&(parm).COMMON, (void(_CODE_PTR_)(pointer))(cmd))
#define RTCSCMD_internal(parm,cmd)   RTCS_cmd_internal(&(parm).COMMON, (void(_CODE_PTR_)(pointer))(cmd))
#define RTCSCMD_smartissue(parm,cmd) RTCS_cmd_smartissue(&(parm).COMMON, (void(_CODE_PTR_)(pointer))(cmd))
#define RTCSCMD_service(pcb,cmd)     RTCS_cmd_service(pcb, (void(_CODE_PTR_)(pointer))(cmd))
#define RTCSCMD_complete(parm,err)   RTCS_cmd_complete(&(parm)->COMMON, err)

#define RTCS_getcfg(prot)        ((RTCS_DATA_PTR)RTCS_get_data())-> prot ## _CFG
#define RTCS_setcfg(prot,cfg)    ((RTCS_DATA_PTR)RTCS_get_data())-> prot ## _CFG = (cfg)

typedef struct rtcs_data {
   uint_32        VERSION;

   pointer        TCPIP_CFG;
   pointer        SOCKET_CFG;
   pointer        IP_CFG;
   pointer        ICMP_CFG;
   pointer        UDP_CFG;
   pointer        TCP_CFG;

   _pool_id       TCPIP_msg_pool;
   _queue_id      TCPIP_qid;
   _rtcs_part     RTCS_PCB_partition;
   _rtcs_part     RTCS_socket_partition;

#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
   pointer        IGMP_CFG;
#endif
#if RTCSCFG_ENABLE_RIP && RTCSCFG_ENABLE_IP4 
   pointer        RIP_CFG;
#endif
#if RTCSCFG_ENABLE_OSPF
   pointer        OSPF_CFG;
#endif

   _rtcs_taskid   TCPIP_TASKID;

#if RTCSCFG_LOG_SOCKET_API || RTCSCFG_LOG_PCB || RTCSCFG_LOGGING
   uint_32        RTCS_LOG_CONTROL;
   uint_32        RTCS_LOG_PROTCOUNT;
   uint_16        RTCS_LOG_PCB[RTCSLOG_PROT_MAX];
   uint_16        RTCS_LOG_TIMER[RTCSLOG_PROT_MAX];
#endif

#if RTCSCFG_ENABLE_NAT
   pointer        NAT_CFG;
#endif
#if RTCSCFG_ENABLE_IPSEC
   pointer        IPSHIELD_CFG;
#endif
#if RTCSCFG_ENABLE_IPIP && RTCSCFG_ENABLE_IP4
   pointer        IPIP_CFG;
#endif
#if RTCSCFG_ENABLE_DNS
   /* Start CR 2170 */
   pointer        DNS_CFG;
   /* End CR 2170 */
#endif

#if RTCSCFG_ENABLE_IP6 //FSL AB
    pointer        IP6_CFG;
    pointer        ICMP6_CFG;
#endif

} RTCS_DATA, _PTR_ RTCS_DATA_PTR;

/*
** externs for RTCS utility procedures
*/

extern void RTCS_seterror
(
   RTCS_ERROR_STRUCT_PTR   errptr,
   uint_32                 errcode,
   uint_32                 errparm
);

extern void RTCS_setsockerror
(
   uint_32     sock,
   uint_32     error
);

extern uint_32 RTCS_cmd_issue      (TCPIP_PARM_PTR, void(_CODE_PTR_)(pointer));
extern uint_32 RTCS_cmd_internal   (TCPIP_PARM_PTR, void(_CODE_PTR_)(pointer));
extern uint_32 RTCS_cmd_smartissue (TCPIP_PARM_PTR, void(_CODE_PTR_)(pointer));
extern boolean RTCS_cmd_service    (RTCSPCB_PTR,    void(_CODE_PTR_)(pointer));
extern void    RTCS_cmd_complete   (TCPIP_PARM_PTR, uint_32);

#endif
/* EOF */
