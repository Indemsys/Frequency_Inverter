#ifndef __icmp_prv_h__
#define __icmp_prv_h__
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
* $FileName: icmp_prv.h$
* $Version : 3.0.6.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   Private definitions for the ICMP protocol layer.
*
*END************************************************************************/

#include "ip_prv.h"     /* For IP_HEADER */


/***************************************
**
** Code macros
**
*/

#define ICMPTYPE_ISQUERY(t)   (((t) == ICMPTYPE_ECHO_REPLY) \
                            || ((t) == ICMPTYPE_ECHO_REQ)   \
                            || (((t) >= ICMPTYPE_TIME_REQ) && ((t) <= ICMPTYPE_AM_REPLY)))


#if RTCSCFG_ENABLE_ICMP_STATS 
   #define IF_ICMP_STATS_ENABLED(x) x
#else
   #define IF_ICMP_STATS_ENABLED(x)
#endif


/***************************************
**
** Type definitions
**
*/

/*
** Generic ICMP packet header
*/
typedef struct icmp_header
{
   uchar    TYPE[1];
   uchar    CODE[1];
   uchar    CHECKSUM[2];
} ICMP_HEADER, _PTR_ ICMP_HEADER_PTR;

/*
** ICMP Redirect packet
*/
typedef struct icmp_err_header
{
   ICMP_HEADER HEAD;
   uchar       DATA[4];
   IP_HEADER   IP;
} ICMP_ERR_HEADER, _PTR_ ICMP_ERR_HEADER_PTR;

/*
** ICMP Echo packet
*/
typedef struct icmp_echo_header
{
   ICMP_HEADER HEAD;
   uchar       ID[2];
   uchar       SEQ[2];
} ICMP_ECHO_HEADER, _PTR_ ICMP_ECHO_HEADER_PTR;

/*
** ICMP Configuration.  This information is persistent for the ICMP layer.
*/
typedef struct icmp_cfg_struct {
#if RTCSCFG_ENABLE_ICMP_STATS
   ICMP_STATS     STATS;
#endif
   ICMP_PARM_PTR  ECHO_PARM_HEAD;
   uint_16        ECHO_SEQ;
   uint_16        RESERVED;

} ICMP_CFG_STRUCT, _PTR_ ICMP_CFG_STRUCT_PTR;


/***************************************
**
** Prototypes
**
*/

extern void ICMP_send_error_internal
(
   uint_8         type,    /* [IN] the type to send */
   uint_8         code,    /* [IN] the code to send */
   uint_32        param,   /* [IN] the icmp parameter */
   IP_HEADER_PTR  iph,     /* [IN] the IP header */
   RTCSPCB_PTR    origpcb, /* [IN] the bad packet pcb */
   uint_32        maxlen   /* [IN[ the max data len to send, 0 = default */
);


#endif
/* EOF */
