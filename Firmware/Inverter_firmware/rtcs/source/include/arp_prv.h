#ifndef __arp_prv_h__
#define __arp_prv_h__
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
* $FileName: arp_prv.h$
* $Version : 3.5.7.0$
* $Date    : Dec-4-2009$
*
* Comments:
*
*   Address Resolution Protocol definitions.
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/

/*
** ARP operation codes
*/
#define ARPOP_REQUEST   1
#define ARPOP_REPLY     2

/*
** ARP protocol types
*/
#define ARPPROT_IP      0x800


/*
** Warning: ARPCACHE_HASH() should really be ((a) % ARPCACHE_SIZE),
** but this is faster when ARPCACHE_SIZE is a power of two.
** Note however, that if ARPCACHE_SIZE must be increased, a power
** of two is NOT a good choice -- an odd number (i.e. anything
** coprime to the size of the subnet) would be better.
*/
#define ARPCACHE_HASH(a)   ((a) & (ARPCACHE_SIZE - 1))

#if (ARP_ENTRY_MAX_QUEUED_PCBS>1)
#define ARP_ENTRY_QUEUE_SIZE(e)    (e->QUEUED_PCBS)
#define ARP_ENTRY_DEQUEUE_PCB(e)    _arp_entry_dequeue_pcb(e)
#define ARP_ENTRY_ENQUEUE_PCB(e,p)  _arp_entry_enqueue_pcb(e,p)
#define ARP_ENTRY_QUEUE_PEEK(e)     _arp_entry_queue_peek(e)
#define ARP_ENTRY_QUEUE_INIT(e)     _arp_entry_queue_init(e)
#define ARP_ENTRY_QUEUE_FULL(e)     (e->QUEUED_PCBS == ARP_ENTRY_MAX_QUEUED_PCBS)
#else
#define ARP_ENTRY_QUEUE_SIZE(e)  ((e->WAITING != NULL)?1:0)
#define ARP_ENTRY_ENQUEUE_PCB(e,p)     (e->WAITING=p)
#define ARP_ENTRY_DEQUEUE_PCB(e)    e->WAITING; e->WAITING=NULL
#define ARP_ENTRY_QUEUE_PEEK(e)       (e->WAITING)
#define ARP_ENTRY_QUEUE_FULL(e)     (e->WAITING != NULL)
#define ARP_ENTRY_QUEUE_INIT(e)     (e->WAITING = NULL)

#endif
// END CR 2233


#if RTCSCFG_ENABLE_ARP_STATS
#define IF_ARP_STATS_ENABLED(x) x
#else
#define IF_ARP_STATS_ENABLED(x)
#endif

/***************************************
**
** Type definitions
**
*/

/*
** Contents of an ARP packet
*/
typedef struct arp_packet
{
   uchar    LINKTYPE[2];
   uchar    PROTTYPE[2];
   uchar    LINKLEN[1];
   uchar    PROTLEN[1];
   uchar    OPCODE[2];
   uchar    LINKSRC[6];
   uchar    PROTSRC[4];
   uchar    LINKDEST[6];
   uchar    PROTDEST[4];
} ARP_PACKET, _PTR_ ARP_PACKET_PTR;


typedef struct arp_entry {

   _ip_address             PADDR;
   uchar                   LADDR[6];   /* ignored when COMPLETE == FALSE */
   uchar                   RESERVED[2];

   IP_IF_PTR               HEAD;
   struct arp_entry _PTR_  PREV;
   struct arp_entry _PTR_  NEXT;

   boolean                 COMPLETE;
   boolean                 AGED;       /* ignored when COMPLETE == FALSE */
   boolean                 HIT;        /* ignored when COMPLETE == FALSE */

// START CR 2233
#if (ARP_ENTRY_MAX_QUEUED_PCBS==1)
   /* There is at most one queued PCB on any incomplete entry */
   RTCSPCB_PTR             WAITING;

#else
   /* There may be multiple queued PCB on any incomplete entry */
   RTCSPCB_PTR             WAITING[ARP_ENTRY_MAX_QUEUED_PCBS];
   uint_32                 QUEUED_PCBS;
#endif
// END CR 2233

   _ip_address             WAITADDR;   /* ignored when RESEND_TIMER inactive */
   RTCS_LINKOPT_STRUCT     WAITOPT;    /* ignored when RESEND_TIMER inactive */

   uint_32                 RESEND_INTERVAL;
   TCPIP_EVENT             RESEND_TIMER;
   TCPIP_EVENT             AGE_TIMER;
   TCPIP_EVENT             EXPIRE_TIMER;

} ARP_ENTRY, _PTR_ ARP_ENTRY_PTR;


/*
** ARP Configuration
**    This information is persistent for the ARP layer.
*/
typedef struct arp_cfg {

#if RTCSCFG_ENABLE_ARP_STATS
   ARP_STATS      STATS;
#endif

   ARP_ENTRY_PTR  CACHE[ARPCACHE_SIZE];

   uchar          LADDR[6];
   uchar          RESERVED[2];

   ARP_PACKET     ARP_REQUEST;

} ARP_CFG, _PTR_ ARP_CFG_PTR;


/***************************************
**
** Global variables
**
*/
extern pointer ARP_freelist;

// START CR 2233
#if (ARP_ENTRY_MAX_QUEUED_PCBS!=1)
extern RTCSPCB_PTR   _arp_entry_dequeue_pcb(ARP_ENTRY_PTR e);
extern boolean       _arp_entry_enqueue_pcb(ARP_ENTRY_PTR e,RTCSPCB_PTR p);
extern RTCSPCB_PTR   _arp_entry_queue_peek(ARP_ENTRY_PTR e);
extern boolean       _arp_entry_queue_init(ARP_ENTRY_PTR e);
#endif
// END CR 2233

#endif
/* EOF */
