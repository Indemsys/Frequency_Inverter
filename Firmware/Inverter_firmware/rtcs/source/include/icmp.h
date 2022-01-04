#ifndef __icmp_h__
#define __icmp_h__
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
* $FileName: icmp.h$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   Definitions for the ICMP protocol layer.
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/

/*
** ICMP message types
*/
#define ICMPTYPE_ECHO_REPLY   0
#define ICMPTYPE_DESTUNREACH  3
#define ICMPTYPE_SRCQUENCH    4
#define ICMPTYPE_REDIRECT     5
#define ICMPTYPE_ECHO_REQ     8
#define ICMPTYPE_TIMEEXCEED   11
#define ICMPTYPE_PARMPROB     12
#define ICMPTYPE_TIME_REQ     13
#define ICMPTYPE_TIME_REPLY   14
#define ICMPTYPE_INFO_REQ     15    /* obsolete */
#define ICMPTYPE_INFO_REPLY   16    /* obsolete */
#define ICMPTYPE_AM_REQ       17
#define ICMPTYPE_AM_REPLY     18

/*
** Destination Unreachable codes
*/
#define ICMPCODE_DU_NET_UNREACH     0
#define ICMPCODE_DU_HOST_UNREACH    1
#define ICMPCODE_DU_PROTO_UNREACH   2
#define ICMPCODE_DU_PORT_UNREACH    3
#define ICMPCODE_DU_NEEDFRAG        4
#define ICMPCODE_DU_SRCROUTE        5
#define ICMPCODE_DU_NET_UNKNOWN     6
#define ICMPCODE_DU_HOST_UNKNOWN    7
#define ICMPCODE_DU_HOST_ISOLATED   8
#define ICMPCODE_DU_NET_PROHIB      9
#define ICMPCODE_DU_HOST_PROHIB     10
#define ICMPCODE_DU_NET_TOS         11
#define ICMPCODE_DU_HOST_TOS        12

/*
** Redirect codes
*/
#define ICMPCODE_RD_NET             0
#define ICMPCODE_RD_HOST            1
#define ICMPCODE_RD_NET_TOS         2
#define ICMPCODE_RD_HOST_TOS        3

/*
** Time Exceeded codes
*/
#define ICMPCODE_TE_TTL             0
#define ICMPCODE_TE_REASM           1


/***************************************
**
** Type definitions
**
*/

/*
** ICMP echo parameters
*/

typedef struct icmp_parm
{
    TCPIP_PARM                      COMMON;
    struct icmp_parm _PTR_          NEXT;
    struct icmp_parm _PTR_ _PTR_    PREV;
    TCPIP_EVENT                     EXPIRE;
    _ip_address                     ipaddress;
    uint_32                         timeout;
    uint_16                         id;
    uint_16                         seq;
    uint_8                          hop_limit;          /* Hop limit orr TTL.*/ 
    pointer                         data_buffer;        /* Pointer to a data buffer (optional). */
    uint_32                         data_buffer_size;   /* Size of a buffer (optional).    */
} ICMP_PARM, _PTR_ ICMP_PARM_PTR;


/***************************************
**
** Prototypes
**
*/

extern uint_32 ICMP_init
(
   void
);

extern void ICMP_send_echo
(
   ICMP_PARM_PTR
);

extern void ICMP_send_error
(
   uint_8         type,    /* [IN] the type to send */
   uint_8         code,    /* [IN] the code to send */
   uint_32        param,   /* [IN] the icmp parameter */
   RTCSPCB_PTR    pcb,     /* [IN] the packet which caused the error */
   int_32         layer    /* [IN] IP layer, relative to current */
);

extern void ICMP_service
(
   RTCSPCB_PTR,
   pointer
);


#endif
/* EOF */
