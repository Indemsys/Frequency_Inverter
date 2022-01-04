#ifndef __udp_prv_h__
#define __udp_prv_h__
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
* $FileName: udp_prv.h$
* $Version : 3.0.6.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the private User Datagram Protocol
*   definitions.  For more details, refer to RFC768.
*
*END************************************************************************/

/*
** This is used to signal the waiting task after 
** socket closed by other tasks. 
*/
#define UDP_SOCKET_ENQUEUED      20
#define UDP_SOCKET_CLOSE         21

#if RTCSCFG_ENABLE_UDP_STATS
#define IF_UDP_STATS_ENABLED(x) x
#else
#define IF_UDP_STATS_ENABLED(x)
#endif

/***************************************
**
** Type definitions
**
*/

typedef struct udp_header {
   uchar    SRC_PORT[2];
   uchar    DEST_PORT[2];
   uchar    LENGTH[2];
   uchar    CHECKSUM[2];
} UDP_HEADER, _PTR_ UDP_HEADER_PTR;

/*
** UDP Configuration.  This information is persistent for the UDP layer.
*/
typedef struct udp_cfg_struct {
#if RTCSCFG_ENABLE_UDP_STATS
   UDP_STATS      STATS;
#endif
   UCB_STRUCT_PTR BOUND_UCB_HEAD;   /* The head of the bound UCB chain      */
   uint_16        LAST_PORT;        /* Last used UDP port                   */
   uint_16        RESERVED0;

   UCB_STRUCT_PTR GROUND_UCB_HEAD;  /* The head of the ground UCB chain     */
   UCB_STRUCT_PTR LBOUND_UCB_HEAD;  /* The head of local IP bound UCB chain */
   UCB_STRUCT_PTR OPEN_UCB_HEAD;    /* The head of the open UCB chain       */

} UDP_CFG_STRUCT, _PTR_ UDP_CFG_STRUCT_PTR;


/***************************************
**
** Prototypes
**
*/

uint_16 UDP_source
(
   RTCSPCB_PTR                /* [IN] packet to find source of */
);


#endif
/* EOF */
