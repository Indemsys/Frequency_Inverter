#ifndef __bootp_h__
#define __bootp_h__
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
* $FileName: bootp.h$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   Bootstrap Protocol definitions.
*
*END************************************************************************/

#include "ip_prv.h"


/***************************************
**
** Constants
**
*/

#define BOOTP_TIMEOUT_MIN     4000     /* 4 sec */
#define BOOTP_TIMEOUT_MAX     64000    /* 64 sec */

#define BOOTPOP_BOOTREQUEST   1
#define BOOTPOP_BOOTREPLY     2

#define BOOTPFLAG_BROADCAST   0x8000

#define BOOTP_MAGIC           0x63825363L

/*
** BOOTP options
*/
#define BOOTPOPT_PAD    0
#define BOOTPOPT_END    255
#define BOOTPOPT_MASK   1


/***************************************
**
** Type definitions
**
*/

/*
** Contents of a BOOTP packet
*/
typedef struct bootp_header
{
   uchar    OP[1];
   uchar    HTYPE[1];
   uchar    HLEN[1];
   uchar    HOPS[1];
   uchar    XID[4];
   uchar    SECS[2];
   uchar    FLAGS[2];
   uchar    CIADDR[4];
   uchar    YIADDR[4];
   uchar    SIADDR[4];
   uchar    GIADDR[4];
   uchar    CHADDR[16];
} BOOTP_HEADER, _PTR_ BOOTP_HEADER_PTR;

typedef struct bootp_data
{
   uchar    SNAME[64];
   uchar    FILE[128];
   uchar    VEND[64];
} BOOTP_DATA, _PTR_ BOOTP_DATA_PTR;

typedef struct bootp_packet
{
   BOOTP_HEADER   HEAD;
   BOOTP_DATA     DATA;
} BOOTP_PACKET, _PTR_ BOOTP_PACKET_PTR;

/*
** BOOTP Configuration
*/
typedef struct bootp_cfg
{
   uint_32        XID;
   uint_32        TIMEOUT;
   uint_32        SECS;
   TCPIP_EVENT    RESEND;
   BOOTP_HEADER   PACKET;
} BOOTP_CFG, _PTR_ BOOTP_CFG_PTR;


/***************************************
**
** Function-specific type definitions
**
*/

/* BOOTP_open() */
typedef struct tcpip_parm_bootp {
   TCPIP_PARM              COMMON;
   _rtcs_if_handle         handle;  /* [IN] the RTCS interface state structure */
   BOOTP_DATA_STRUCT_PTR   data;    /* [IN/OUT] BOOTP parameters */
   BOOTP_CFG               config;
} TCPIP_PARM_BOOTP;


/***************************************
**
** Prototypes
**
*/

void BOOTP_open
(
   TCPIP_PARM_BOOTP _PTR_ parms
);

uint_32 BOOTP_close
(
   IP_IF_PTR                     /* [IN]     IP interface structure */
);

boolean BOOTP_send
(
   TCPIP_EVENT_PTR               /* [IN/OUT] the resend event       */
);

void BOOTP_service
(
   RTCSPCB_PTR                   /* [IN]     BOOTREPLY packet       */
);


#endif
/* EOF */
