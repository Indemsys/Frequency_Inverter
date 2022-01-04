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
* $FileName: arp.c$
* $Version : 3.8.15.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*   This file contains the implementation of the
*   ARP server.
*   Limitations:  Although ARP can be used over any multiple-access
*   medium, this implementation will work only over an
*   Ethernet link layer.
*
*END************************************************************************/

#include <rtcs.h>

#include <enet.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "arp.h"
#include "arp_prv.h"
#include "ip-e.h"
#include "arpif.h"

#if RTCSCFG_ENABLE_IP4

/* Functions for accessing the ARP cache */
static ARP_ENTRY_PTR ARP_find   (IP_IF_PTR, _ip_address);
static ARP_ENTRY_PTR ARP_insert (IP_IF_PTR, _ip_address);
static void          ARP_delete (ARP_ENTRY_PTR);

/* The various timer events */
static boolean ARP_send   (TCPIP_EVENT_PTR);
static boolean ARP_age    (TCPIP_EVENT_PTR);
static boolean ARP_expire (TCPIP_EVENT_PTR);

/* Macros */


#define ARP_REFRESH(a)                                         \
         (a)->RESEND_INTERVAL = RTCSCFG_ARPTIME_RESEND_MIN;            \
         (a)->RESEND_TIMER.TIME = 0;                           \
         (a)->EXPIRE_TIMER.TIME = RTCSCFG_ARPTIME_EXPIRE_INCOMPLETE;   \
         TCPIP_Event_add(&(a)->RESEND_TIMER);                  \
         TCPIP_Event_add(&(a)->EXPIRE_TIMER)

#define ARP_COMPLETE(a,l)                                      \
         _mem_copy(l, (a)->LADDR, sizeof((a)->LADDR));         \
         (a)->COMPLETE = TRUE;                                 \
         (a)->AGED     = FALSE;                                \
         (a)->HIT      = FALSE;                                \
         (a)->AGE_TIMER.TIME = RTCSCFG_ARPTIME_EXPIRE_COMPLETE         \
                             - RTCSCFG_ARPTIME_EXPIRE_INCOMPLETE;      \
         (a)->EXPIRE_TIMER.TIME = RTCSCFG_ARPTIME_EXPIRE_COMPLETE;     \
         TCPIP_Event_add(&(a)->AGE_TIMER);                     \
         TCPIP_Event_add(&(a)->EXPIRE_TIMER)

/*
** Each ARP cache entry basically has four states:
** complete, aged/quiet, aged/active, incomplete.
**
** For every entry, there exists three timers.
**
**    The resend timer periodically causes an ARP request
**    to be sent.
**
**    The age timer causes complete->aged/quiet and
**    aged->incomplete transitions.
**
**    The expire timer deletes an entry.
**
** For each state, the following timers are active.
**
**    complete       age, expire
**    aged/quiet     age, expire
**    aged/active    resend, age, expire
**    incomplete     resend, expire
**
** Additionally, you can determine the state of an entry from the
** following fields:
**
**                   COMPLETE    AGED     HIT
**    complete       TRUE        FALSE    xx
**    aged/quiet     TRUE        TRUE     FALSE
**    aged/active    TRUE        TRUE     TRUE
**    incomplete     FALSE       xx       xx
**
**
** Here's how it works:
**
** Each entry starts as either complete or incomplete, depending
** on who initiated communication (us or the peer).
**
** Complete entries become aged/quiet after 17 minutes, and
** expire after 20 minutes.
** Aged entries become incomplete after 3 minutes.
**
** When an entry becomes aged, we try to refresh it before it expires.
** However, we don't want to keep entries for unused hosts.
**
** Once the entry ages, if it is not used in the next 3 minutes, it
** will be deleted when the expire timer fires.
**
** If the entry is used in the next 3 minutes, it becomes aged/active.
** We start the resend timer and reset the expire timer to 3 minutes
** after the first hit to give the peer a reasonable chance to reply.
** The age timer remains unchanged -- it will still fire at the same
** time, and make the entry incomplete.
**
** Finally, incomplete entries periodically send ARP requests until
** they expire.
*/

#if (ARP_ENTRY_MAX_QUEUED_PCBS!=1)

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _arp_entry_queue_init
*  Returned Value : pcb or NULL
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/
boolean       _arp_entry_queue_init(ARP_ENTRY_PTR entry_ptr)
{
   uint_32  i;

   for (i=0;i<ARP_ENTRY_MAX_QUEUED_PCBS;i++) {
      entry_ptr->WAITING[i] = NULL;
   }
   entry_ptr->QUEUED_PCBS=0;

   return TRUE;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _arp_entry_enqueue_pcb
*  Returned Value : boolean
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/
boolean   _arp_entry_enqueue_pcb(ARP_ENTRY_PTR entry_ptr,RTCSPCB_PTR rtcs_pcb)
{
   uint_32 i;

   if (entry_ptr->QUEUED_PCBS == ARP_ENTRY_MAX_QUEUED_PCBS) {
      return FALSE;
   } 

   for (i=entry_ptr->QUEUED_PCBS;i>0;i--) {
      entry_ptr->WAITING[i] = entry_ptr->WAITING[i-1];
   }
   entry_ptr->WAITING[0]=rtcs_pcb;
   entry_ptr->QUEUED_PCBS++;

   return TRUE;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _arp_entry_queue_peek
*  Returned Value : pcb or NULL
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/
RTCSPCB_PTR   _arp_entry_queue_peek(ARP_ENTRY_PTR entry_ptr)
{
   RTCSPCB_PTR rtcs_pcb = NULL;

   if (entry_ptr->QUEUED_PCBS) {
      rtcs_pcb = entry_ptr->WAITING[entry_ptr->QUEUED_PCBS-1];
   }

   return rtcs_pcb;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _arp_entry_dequeue_pcb
*  Returned Value : oldest pcb or NULL
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/

RTCSPCB_PTR   _arp_entry_dequeue_pcb(ARP_ENTRY_PTR entry_ptr)
{
   RTCSPCB_PTR rtcs_pcb = NULL;

   if (entry_ptr->QUEUED_PCBS) {
      entry_ptr->QUEUED_PCBS--;
      rtcs_pcb = entry_ptr->WAITING[entry_ptr->QUEUED_PCBS];
      entry_ptr->WAITING[entry_ptr->QUEUED_PCBS]=NULL;
   }

   return rtcs_pcb;
}

#endif

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_open
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the ARP Server on an interface.
*
*END*-----------------------------------------------------------------*/

uint_32 ARP_open
   (
      IP_IF_PTR   if_ptr
         /* [IN] IP interface structure */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg;
   ARP_PACKET_PTR packet;

   /* Allocate an ARP cache and a statistics structure */
   arp_cfg = RTCS_mem_alloc_zero(sizeof(ARP_CFG));
   if (!arp_cfg) {
      return RTCSERR_ARP_CFG;
   } /* Endif */
   _mem_set_type(arp_cfg, MEM_TYPE_ARP_CFG);

   /* Get the interface's Ethernet address */
   #if BSP_ENET_DEVICE_COUNT > 0
   ENET_get_address(if_ptr->HANDLE, arp_cfg->LADDR);
   #endif
   /* Prebuild an ARP request */
   packet = &arp_cfg->ARP_REQUEST;
   htons(packet->LINKTYPE, ARPLINK_ETHERNET);
   htons(packet->PROTTYPE, ARPPROT_IP);
   htonc(packet->LINKLEN,  6);
   htonc(packet->PROTLEN,  4);
   htons(packet->OPCODE,   ARPOP_REQUEST);
   htone(packet->LINKSRC,  arp_cfg->LADDR);
   _mem_zero(packet->LINKDEST, 6);

   if_ptr->ARP = arp_cfg;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_close
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Shuts down the ARP Server on an interface.
*
*END*-----------------------------------------------------------------*/

uint_32 ARP_close
   (
      IP_IF_PTR   if_ptr
         /* [IN] IP interface structure */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg;
   uint_32        hash;
   ARP_ENTRY_PTR  entry, nextentry;

   arp_cfg = if_ptr->ARP;
   for (hash = 0; hash < ARPCACHE_SIZE; hash++) {
      entry = arp_cfg->CACHE[hash];
      while (entry) {

         /*
         ** Note: this could be replaced with
         **    TCPIP_Event_cancel(&entry->EXPIRE_TIMER);
         **    ARP_expire(&entry->EXPIRE_TIMER);
         ** but this is faster
         */

         /* Cancel all timers, free all packets */
         TCPIP_Event_cancel(&entry->RESEND_TIMER);
         TCPIP_Event_cancel(&entry->AGE_TIMER);
         TCPIP_Event_cancel(&entry->EXPIRE_TIMER);

         while (ARP_ENTRY_QUEUE_SIZE(entry)) {
            RTCSPCB_PTR rtcs_pcb = ARP_ENTRY_DEQUEUE_PCB(entry);
            RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_ARP_CANT_RESOLVE);
            RTCSPCB_free(rtcs_pcb);
         } /* Endif */
         /* Enqueue the entry onto the free list */
         nextentry = entry->NEXT;
         entry->NEXT = ARP_freelist;
         ARP_freelist = entry;
         entry = nextentry;
      } /* Endwhile */
   } /* Endfor */

   _mem_free(arp_cfg);
   if_ptr->ARP = NULL;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_find
*  Returned Value : the ARP entry if found, NULL otherwise
*  Comments       :
*        Searches the ARP cache for an entry.
*
*END*-----------------------------------------------------------------*/

static ARP_ENTRY_PTR ARP_find
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    paddr
         /* [IN] protocol address */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = if_ptr->ARP;
   ARP_ENTRY_PTR  arp_ptr;
   uint_32        hash;

   /* Locate the IP address in the cache */
   hash = ARPCACHE_HASH(paddr);
   for (arp_ptr = arp_cfg->CACHE[hash];
        arp_ptr;
        arp_ptr = arp_ptr->NEXT) {
      if (arp_ptr->PADDR == paddr) {
         break;
      } /* Endif */
   } /* Endfor */

   return arp_ptr;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_is_complete
*  Returned Value : TRUE if the ARP entry was found, FALSE otherwise
*  Comments       :
*        Searches the ARP cache for an entry.
*
*END*-----------------------------------------------------------------*/

boolean  ARP_is_complete
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    addr
         /* [IN] the IP address to test */
   )
{ /* Body */

    ARP_ENTRY_PTR  arp_ptr;
  
  arp_ptr = ARP_find (if_ptr, addr);
  
  if (arp_ptr) {    
    return (arp_ptr->COMPLETE);
  }
  return FALSE;
 
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_kill_entry
*  Returned Value : Always returns FALSE (inherited from ARP_expire)
*  Comments       :
*        Searches the ARP cache for an entry. Removes it (and its PCB)
*        if found.
*
*END*-----------------------------------------------------------------*/

boolean  ARP_kill_entry
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    addr
         /* [IN] the IP address to test */
   )
{ /* Body */

    ARP_ENTRY_PTR   arp_ptr;
    TCPIP_EVENT     dummy_event;
  
  arp_ptr = ARP_find (if_ptr, addr);
  
  if (arp_ptr) {      
     dummy_event.TIME       = 0;
     dummy_event.EVENT      = NULL;
     dummy_event.PRIVATE    = arp_ptr;
     dummy_event.NEXT       = NULL;
     
     return (ARP_expire(&dummy_event));
  }  
  return FALSE;
 
} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_find_waiting
*  Returned Value : a pointer to the PCB waiting to be sent for the ARP 
*                 : table entry related to if_ptr and paddr.
*                 
*  Comments       :
*        Searches the ARP cache for an entry. Returns a pointer to the 
*        related waiting, outbound packet or NULL if none exists
*
*        It is important that the calling application consider this
*        PCB read only. The arp system has the responsibility to 
*        expire and release the PCB. If you want to free the waiting 
*        PCB, you should use ARP_kill_entry()
*
END*-----------------------------------------------------------------*/

RTCSPCB_PTR  ARP_find_waiting
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    addr
         /* [IN] the IP address to test */
   )
{ /* Body */

   ARP_ENTRY_PTR   arp_ptr;
  
   arp_ptr = ARP_find (if_ptr, addr);

   if (arp_ptr) {
      return (RTCSPCB_PTR)ARP_ENTRY_QUEUE_PEEK(arp_ptr);
   }
  
   return NULL;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_insert
*  Returned Value : the new ARP entry
*  Comments       :
*        Create an ARP entry.  Note: it is the caller's responsibility
*        to initialize the ARP_ENTRY's fields.
*
*END*-----------------------------------------------------------------*/

static ARP_ENTRY_PTR ARP_insert
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    paddr
         /* [IN] protocol address */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = if_ptr->ARP;
   ARP_ENTRY_PTR  arp_ptr;
   uint_32        i, hash;

   /* Allocate a block of entries if none are left */
   if (!ARP_freelist) {
      arp_ptr = RTCS_mem_alloc_zero(ARPALLOC_SIZE * sizeof(ARP_ENTRY));
      if (arp_ptr) {
         _mem_set_type(arp_ptr, MEM_TYPE_ARP_ENTRY);
         for (i = 0; i < ARPALLOC_SIZE; i++) {
            arp_ptr->NEXT = ARP_freelist;
            ARP_freelist = arp_ptr;
            arp_ptr++;
         } /* Endfor */
      } /* Endif */
   } /* Endif */

   /* Dequeue a free entry */
   arp_ptr = ARP_freelist;
   if (!arp_ptr) {
      IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_ALLOCS_FAILED++);
      return NULL;
   } /* Endif */
   ARP_freelist = arp_ptr->NEXT;

   arp_ptr->PADDR                = paddr;
   arp_ptr->RESEND_TIMER.EVENT   = ARP_send;
   arp_ptr->RESEND_TIMER.PRIVATE = arp_ptr;
   arp_ptr->AGE_TIMER.EVENT      = ARP_age;
   arp_ptr->AGE_TIMER.PRIVATE    = arp_ptr;
   arp_ptr->EXPIRE_TIMER.EVENT   = ARP_expire;
   arp_ptr->EXPIRE_TIMER.PRIVATE = arp_ptr;

   /* Enqueue the new entry in the two-way list */
   hash = ARPCACHE_HASH(paddr);
   arp_ptr->HEAD = if_ptr;
   arp_ptr->PREV = NULL;
   arp_ptr->NEXT = arp_cfg->CACHE[hash];
   if (arp_ptr->NEXT) {
      arp_ptr->NEXT->PREV = arp_ptr;
   } /* Endif */
   arp_cfg->CACHE[hash] = arp_ptr;

   ARP_ENTRY_QUEUE_INIT(arp_ptr);
   return arp_ptr;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_delete
*  Returned Value : void
*  Comments       :
*        Delete an ARP entry.
*
*END*-----------------------------------------------------------------*/

static void ARP_delete
   (
      ARP_ENTRY_PTR  arp_ptr
         /* [IN] ARP entry */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = arp_ptr->HEAD->ARP;
   uint_32        hash;

   /* Dequeue the entry from the two-way list */
   hash = ARPCACHE_HASH(arp_ptr->PADDR);
   if (arp_ptr->NEXT) {
      arp_ptr->NEXT->PREV = arp_ptr->PREV;
   } /* Endif */
   if (arp_ptr->PREV) {
      arp_ptr->PREV->NEXT = arp_ptr->NEXT;
   } /* Endif */
   if (arp_cfg->CACHE[hash] == arp_ptr) {
      arp_cfg->CACHE[hash] = arp_ptr->NEXT;
   } /* Endif */

   /* Enqueue the entry onto the free list */
   arp_ptr->NEXT = ARP_freelist;
   ARP_freelist = arp_ptr;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_send
*  Returned Value :
*  Comments       :
*        Called by the Timer.  Send an ARP request
*
*END*-----------------------------------------------------------------*/

static boolean ARP_send
   (
      TCPIP_EVENT_PTR   event
         /* [IN/OUT] the refresh event */
   )
{ /* Body */
   ARP_ENTRY_PTR  arp_ptr = event->PRIVATE;
   ARP_CFG_PTR    arp_cfg = arp_ptr->HEAD->ARP;
   RTCSPCB_PTR    rtcs_pcb;
   ARP_PACKET_PTR packet;
   uint_32        error;

   IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_TOTAL++);
   IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_TX_REQUESTS++);

   /*
   ** Obviously, the gotos below aren't necessary; they're there to
   ** prevent the indentation level from getting ridiculously deep.
   */

   rtcs_pcb = RTCSPCB_alloc_send();
   if (!rtcs_pcb) {
      IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_MISSED++);
      goto arpsend_fail;
   } /* Endif */

   error = RTCSPCB_insert_header(rtcs_pcb, sizeof(ARP_PACKET));
   if (error) {
      IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_ERRORS++);
      IF_ARP_STATS_ENABLED(RTCS_seterror(&arp_cfg->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
      RTCSLOG_PCB_FREE(rtcs_pcb, error);
      RTCSPCB_free(rtcs_pcb);
      goto arpsend_fail;
   } /* Endif */

   RTCSLOG_PCB_WRITE(rtcs_pcb, RTCS_LOGCTRL_ARP(IPIFTYPE_ETHERNET), 0);

   packet = (ARP_PACKET_PTR)RTCSPCB_DATA(rtcs_pcb);
   _mem_copy(&arp_cfg->ARP_REQUEST, packet, sizeof(*packet));
   htonl(packet->PROTSRC,  arp_ptr->WAITADDR);
   htonl(packet->PROTDEST, arp_ptr->PADDR);

   rtcs_pcb->LINK_OPTIONS.TX = arp_ptr->WAITOPT;
   #if BSP_ENET_DEVICE_COUNT > 0
   error = IPE_send_ARP_bcast(arp_ptr->HEAD, rtcs_pcb);
   #endif
   if (error) {
      IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_ERRORS++);
      IF_ARP_STATS_ENABLED(RTCS_seterror(&arp_cfg->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
      goto arpsend_fail;
   } /* Endif */

arpsend_fail:
   arp_ptr->RESEND_INTERVAL *= 2;
   if (arp_ptr->RESEND_INTERVAL > RTCSCFG_ARPTIME_RESEND_MAX) {
      arp_ptr->RESEND_INTERVAL = RTCSCFG_ARPTIME_RESEND_MAX;
   } /* Endif */
   event->TIME = arp_ptr->RESEND_INTERVAL;
   return TRUE;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_age
*  Returned Value :
*  Comments       :
*        Called by the Timer.  Age an ARP cache entry.
*
*END*-----------------------------------------------------------------*/

static boolean ARP_age
   (
      TCPIP_EVENT_PTR   event
         /* [IN/OUT] the aging event */
   )
{ /* Body */
   ARP_ENTRY_PTR  arp_ptr = event->PRIVATE;

   /* Complete entries become aged/quiet */
   if (!arp_ptr->AGED) {
      arp_ptr->AGED = TRUE;
      arp_ptr->HIT  = FALSE;
      event->TIME   = RTCSCFG_ARPTIME_EXPIRE_INCOMPLETE;
      return TRUE;

   /* Aged entries become incomplete */
   } else {
      arp_ptr->COMPLETE = FALSE;
      return FALSE;

   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_expire
*  Returned Value :
*  Comments       :
*        Called by the Timer.  Expire an ARP cache entry.
*
*END*-----------------------------------------------------------------*/

static boolean ARP_expire
   (
      TCPIP_EVENT_PTR   event
         /* [IN/OUT] the expiration event */
   )
{ /* Body */
   ARP_ENTRY_PTR  arp_ptr = event->PRIVATE;
   ARP_CFG_PTR    arp_cfg = arp_ptr->HEAD->ARP;

   /* When an entry expires, free it and any packets queued on it */
   while (ARP_ENTRY_QUEUE_SIZE(arp_ptr)) {
      RTCSPCB_PTR rtcs_pcb = ARP_ENTRY_DEQUEUE_PCB(arp_ptr);

      IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_PKT_DISCARDS++);
      IF_ARP_STATS_ENABLED(arp_ptr->HEAD->STATS.COMMON.ST_TX_DISCARDED++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_ARP_CANT_RESOLVE);

      // pass error back to application
      if (rtcs_pcb->UDP_REQUEST) {
         RTCS_cmd_complete(rtcs_pcb->UDP_REQUEST, RTCSERR_ARP_CANT_RESOLVE);
         rtcs_pcb->UDP_REQUEST = NULL;
      } /* Endif */

      RTCSPCB_free(rtcs_pcb);
   } /* Endif */

   TCPIP_Event_cancel(&arp_ptr->RESEND_TIMER);
   TCPIP_Event_cancel(&arp_ptr->AGE_TIMER);
   
   /* 
   ** NOTE: ARP_expire may be called either from the ARP_expire timer event
   ** or possibly from ARP_kill_entry(). If we are called from ARP_kill_entry()
   ** then we MUST cancel the ARP_expire timer event. If we are called from 
   ** the ARP_expire timer event, then canceling the ARP_expire timer event 
   ** here is useless but harmless.
   ** So leave the cancle(expire timer) here even though it may look odd.
   */
   TCPIP_Event_cancel(&arp_ptr->EXPIRE_TIMER);
   
   ARP_delete(arp_ptr);

   return FALSE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_resolve
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Called by IP-E.  Translates an IP address to a physical address.
*
*END*-----------------------------------------------------------------*/

uint_32 ARP_resolve
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      RTCSPCB_PTR    rtcs_pcb,
         /* [IN] packet to send */
      _ip_address    isrc,
         /* [IN] source address */
      _ip_address    idest
         /* [IN] destination address */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = if_ptr->ARP;
   ARP_ENTRY_PTR  arp_ptr;

   /* First check the ARP cache */
   arp_ptr = ARP_find(if_ptr, idest);

   /* If there's no entry, create an incomplete one */
   if (!arp_ptr) {

      arp_ptr = ARP_insert(if_ptr, idest);
      if (!arp_ptr) {
         IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_MISSED++);
         IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_PKT_DISCARDS++);
         RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_ARP_CANT_RESOLVE);
         RTCSPCB_free(rtcs_pcb);

      } else {
         ARP_ENTRY_ENQUEUE_PCB(arp_ptr, rtcs_pcb);
         arp_ptr->WAITADDR = isrc;
         arp_ptr->WAITOPT  = rtcs_pcb->LINK_OPTIONS.TX;
         arp_ptr->COMPLETE = FALSE;
         ARP_REFRESH(arp_ptr);
      } /* Endif */

      IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_CACHE_MISSES++);
      return RTCS_OK;

   /* If there's an incomplete entry, queue the packet */
   } else if (!arp_ptr->COMPLETE) {

      if (ARP_ENTRY_QUEUE_FULL(arp_ptr)) {
         RTCSPCB_PTR oldest_rtcs_pcb = ARP_ENTRY_DEQUEUE_PCB(arp_ptr);

         IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_PKT_DISCARDS++);
         IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_DISCARDED++);
         RTCSLOG_PCB_FREE(oldest_rtcs_pcb, RTCSERR_ARP_CANT_RESOLVE);
         RTCSPCB_free(oldest_rtcs_pcb);
      } /* Endif */
      ARP_ENTRY_ENQUEUE_PCB(arp_ptr, rtcs_pcb);
      arp_ptr->WAITADDR = isrc;
      arp_ptr->WAITOPT  = rtcs_pcb->LINK_OPTIONS.TX;
      IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_CACHE_MISSES++);
      return RTCS_OK;
   
   } /* Endif */
   
   /* If there's a complete entry, send the packet */
   /* If the entry is about to expire, try to refresh it */
   if (arp_ptr->AGED && !arp_ptr->HIT) {
      arp_ptr->HIT = TRUE;
      TCPIP_Event_cancel(&arp_ptr->EXPIRE_TIMER);
      arp_ptr->WAITADDR = isrc;
      arp_ptr->WAITOPT  = rtcs_pcb->LINK_OPTIONS.TX;
      ARP_REFRESH(arp_ptr);
   } /* Endif */

   IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_CACHE_HITS++);
   #if BSP_ENET_DEVICE_COUNT > 0
   return IPE_send_IP_ucast(if_ptr, rtcs_pcb, arp_ptr->LADDR);
   #else
   return RTCS_OK;
   #endif 
} /* Endbody */


/*FUNCTION*------------------------------------------------------------- *
 * Function Name : ARP_gratuitous_send
 * Returned Value :
 * Comments :
 * Send a gratuitous ARP reply
 * 
 *END*-----------------------------------------------------------------*/ 
 
static boolean ARP_gratuitous_send 
( 
   TCPIP_EVENT_PTR event 
   /* [IN/OUT] the refresh event */ ) 
{ /* Body */ 
   ARP_ENTRY_PTR arp_ptr = event->PRIVATE; 
   ARP_CFG_PTR arp_cfg = arp_ptr->HEAD->ARP; 
   RTCSPCB_PTR rtcs_pcb; 
   ARP_PACKET_PTR packet; 
   uint_32 error; 

   IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_TOTAL++); 
   IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_TX_REQUESTS++); 

   do {
      rtcs_pcb = RTCSPCB_alloc_send(); 
      if (!rtcs_pcb) { 
         IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_MISSED++); 
         break; 
      } /* Endif */ 
   
      error = RTCSPCB_insert_header(rtcs_pcb, sizeof(ARP_PACKET)); 
      if (error) { 
         IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_ERRORS++); 
         IF_ARP_STATS_ENABLED(RTCS_seterror(&arp_cfg->STATS.ERR_TX, error, (uint_32)rtcs_pcb)); 
         RTCSLOG_PCB_FREE(rtcs_pcb, error); 
         RTCSPCB_free(rtcs_pcb); 
         break; 
      } /* Endif */ 
   
      RTCSLOG_PCB_WRITE(rtcs_pcb, RTCS_LOGCTRL_ARP(IPIFTYPE_ETHERNET), 0); 
      packet = (ARP_PACKET_PTR)RTCSPCB_DATA(rtcs_pcb); 
      _mem_copy(&arp_cfg->ARP_REQUEST, packet, sizeof(*packet));
      
      htonl(packet->PROTSRC, arp_ptr->WAITADDR); 
      htonl(packet->PROTDEST, arp_ptr->PADDR); 
      rtcs_pcb->LINK_OPTIONS.TX = arp_ptr->WAITOPT; 
   
      // Change opcode to a reply and send. 
      htons(packet->OPCODE, ARPOP_REPLY);
      #if BSP_ENET_DEVICE_COUNT > 0
      error = IPE_send_ARP_bcast(arp_ptr->HEAD, rtcs_pcb); 
      #endif
   
      if (error) { 
         IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_ERRORS++); 
         IF_ARP_STATS_ENABLED(RTCS_seterror(&arp_cfg->STATS.ERR_TX, error, (uint_32)rtcs_pcb)); 
         break; 
      } /* Endif */ 
   } while (0);
    
   arp_ptr->RESEND_INTERVAL *= 2; 
   
   if (arp_ptr->RESEND_INTERVAL > RTCSCFG_ARPTIME_RESEND_MAX) { 
      arp_ptr->RESEND_INTERVAL = RTCSCFG_ARPTIME_RESEND_MAX; 
   } /* Endif */ 
   
   event->TIME = arp_ptr->RESEND_INTERVAL; 

   return TRUE; 
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_request
*  Returned Value : RTCS_OK or error code
*  Comments       :
*                 : send an arp request for a given IP.
*
*END*-----------------------------------------------------------------*/

uint_32 ARP_request
   (
      IP_IF_PTR      if_ptr,
         /* [IN] IP interface structure */
      _ip_address    isrc,
         /* [IN] source address */
      _ip_address    idest
         /* [IN] destination address */
   )
{ /* Body */
   ARP_CFG_PTR    arp_cfg = if_ptr->ARP;
   ARP_ENTRY_PTR  arp_ptr;
 
   /* First check the ARP cache */
   arp_ptr = ARP_find(if_ptr, idest);

   /* If there's no entry, create an incomplete one */
   if (!arp_ptr) {
   
      IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_CACHE_MISSES++);
      arp_ptr = ARP_insert(if_ptr, idest);
      if (!arp_ptr) {
         IF_IPIF_STATS_ENABLED(if_ptr->STATS.COMMON.ST_TX_MISSED++);
         return RTCSERR_OUT_OF_MEMORY;
      } else {
         ARP_ENTRY_QUEUE_INIT(arp_ptr);  
         arp_ptr->WAITADDR = isrc;     
         arp_ptr->COMPLETE = FALSE;         
         
         /* send request now (don't queue it for later) */                                  
         ARP_send(&(arp_ptr->RESEND_TIMER));
      } /* Endif */   
   } /* Endif */

   /* Check if we should also do a gratuitous send of a reply packet */
   if (isrc == idest) {
      /* send request now (don't queue it for later) */ 
      ARP_gratuitous_send(&(arp_ptr->RESEND_TIMER)); 
   } /* Endif */ 
   
   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_service
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Processes received ARP packets.
*
*END*-----------------------------------------------------------------*/

void ARP_service
   (
      RTCSPCB_PTR    rtcs_pcb
         /* [IN] ARP packet */
   )
{ /* Body */
   IP_IF_PTR      if_ptr;
   ARP_CFG_PTR    arp_cfg;
   ARP_ENTRY_PTR  arp_ptr;
   ARP_PACKET_PTR packet;
   uint_16        opcode;
   uchar          rladdr[6];
   _ip_address    lpaddr, rpaddr;
   boolean        merge;
   uint_32        error;

   if_ptr = rtcs_pcb->IFSRC;
   arp_cfg = if_ptr->ARP;
   IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_RX_TOTAL++);
   packet = (ARP_PACKET_PTR)RTCSPCB_DATA(rtcs_pcb);
   opcode = ntohs(packet->OPCODE);

   RTCSLOG_PCB_READ(rtcs_pcb, RTCS_LOGCTRL_ARP(IPIFTYPE_ETHERNET), 0);

   /* Validate the packet */
   if ((ntohs(packet->LINKTYPE) != ARPLINK_ETHERNET)
    || (ntohs(packet->PROTTYPE) != ARPPROT_IP)
    || (ntohc(packet->LINKLEN)  != 6)
    || (ntohc(packet->PROTLEN)  != 4)
    || ((opcode != ARPOP_REQUEST)
     && (opcode != ARPOP_REPLY))) {
      IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_RX_DISCARDED++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCSERR_ARP_BAD_HEADER);
      RTCSPCB_free(rtcs_pcb);
      return;
   } /* Endif */

   ntohe(packet->LINKSRC, rladdr);
   rpaddr = ntohl(packet->PROTSRC);
   lpaddr = ntohl(packet->PROTDEST);

   /* If we have an entry, update it */
   arp_ptr = ARP_find(if_ptr, rpaddr);
   if (!arp_ptr) {
      merge = FALSE;
   } else {
      TCPIP_Event_cancel(&arp_ptr->RESEND_TIMER);
      TCPIP_Event_cancel(&arp_ptr->AGE_TIMER);
      TCPIP_Event_cancel(&arp_ptr->EXPIRE_TIMER);
      ARP_COMPLETE(arp_ptr, rladdr);

      while (ARP_ENTRY_QUEUE_SIZE(arp_ptr)) {
         RTCSPCB_PTR queued_rtcs_pcb = ARP_ENTRY_DEQUEUE_PCB(arp_ptr);

         #if BSP_ENET_DEVICE_COUNT > 0
         error = IPE_send_IP_ucast(if_ptr,queued_rtcs_pcb, rladdr);
         #endif
      } /* Endif */

      merge = TRUE;
   } /* Endif */

   if (IP_is_local(if_ptr, lpaddr) || ARP_do_proxy(if_ptr, lpaddr)) {

      /* If we're the target and we don't have an entry, create one */
      if (!merge) {
         arp_ptr = ARP_insert(if_ptr, rpaddr);
         if (arp_ptr) {
            ARP_COMPLETE(arp_ptr, rladdr);
            ARP_ENTRY_QUEUE_INIT(arp_ptr);
         } else {
            IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_RX_MISSED++);
         } /* Endif */
      } /* Endif */

      /* If it's a request for us, reply */
    if (opcode == ARPOP_REQUEST)
    {
        IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_RX_REQUESTS++);
        IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_TOTAL++);
        IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_TX_REPLIES++);

        RTCSLOG_PCB_WRITE(rtcs_pcb, RTCS_LOGCTRL_ARP(IPIFTYPE_ETHERNET), 0);

        htons(packet->OPCODE, ARPOP_REPLY);
        htone(packet->LINKSRC,  arp_cfg->LADDR);
        htonl(packet->PROTSRC,  lpaddr);
        htone(packet->LINKDEST, rladdr);
        htonl(packet->PROTDEST, rpaddr);
        
        /* rtcs_pcb was allocated for receiving, we need to convert it for sending.
        * RTCSPCB_fork() failing isn't a serious error, so we don't check
        * the error code.*/ 
        RTCSPCB_fork(rtcs_pcb); 
        
        #if BSP_ENET_DEVICE_COUNT > 0 
        error = IPE_send_ARP_ucast(if_ptr, rtcs_pcb, rladdr);
        if (error) {
            IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_TX_ERRORS++);
            IF_ARP_STATS_ENABLED(RTCS_seterror(&arp_cfg->STATS.ERR_TX, error, (uint_32)rtcs_pcb));
            return;
        } 
        #endif
    } 
    else
    {
        IF_ARP_STATS_ENABLED(arp_cfg->STATS.ST_RX_REPLIES++);
        RTCSLOG_PCB_FREE(rtcs_pcb, RTCS_OK);
        RTCSPCB_free(rtcs_pcb);
    } 

   } else {
      IF_ARP_STATS_ENABLED(arp_cfg->STATS.COMMON.ST_RX_DISCARDED++);
      RTCSLOG_PCB_FREE(rtcs_pcb, RTCS_OK);
      RTCSPCB_free(rtcs_pcb);
   } /* Endif */

} /* Endbody */


#if 1

#include <fio.h>
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_display_entry
*  Returned Value : 
*  Comments       :
*        Display the ARP cache for an interface.
*
*END*-----------------------------------------------------------------*/


static void ARP_display_entry
   (
      ARP_ENTRY_PTR  arp_ptr
   )
{
   uint_32 i;

   printf("%03d.%03d.%03d.%03d ",IPBYTES(arp_ptr->PADDR));
   for (i=0;i<6;i++) printf("%02x%c",arp_ptr->LADDR[i],(i==5?' ':':'));

    if (arp_ptr->COMPLETE) {
       printf("C%c%c ", arp_ptr->AGED?'A':' ',arp_ptr->HIT?'H':' ' ); 
   } else {
       printf("I--" );
   }

    printf("Resend=%d, Age=%d, Expire=%d\n", 
        arp_ptr->RESEND_TIMER.TIME, arp_ptr->AGE_TIMER.TIME, arp_ptr->EXPIRE_TIMER.TIME);
}



void ARPIF_add
   (
      ARPIF_PARM_PTR  parms
   )
{
    ARP_ENTRY_PTR  arp_ptr; 
   
    if (parms->ihandle==NULL)
    { 
        RTCSCMD_complete(parms, RTCS_HANDLE_ERROR); 
    } 
  
    arp_ptr = ARP_find(parms->ihandle, parms->PADDR); 
    if (!arp_ptr)
    { 
        arp_ptr = ARP_insert(parms->ihandle, parms->PADDR); 
        if (!arp_ptr)
        { 
            RTCSCMD_complete(parms, RTCSERR_OUT_OF_MEMORY); 
        } 
    } 
    else
    { 
        /* Merge. */
        TCPIP_Event_cancel(&arp_ptr->RESEND_TIMER); 
        TCPIP_Event_cancel(&arp_ptr->AGE_TIMER); 
        TCPIP_Event_cancel(&arp_ptr->EXPIRE_TIMER); 
    } 
    ARP_COMPLETE(arp_ptr, parms->LADDR);   
    
    RTCSCMD_complete(parms, RTCS_OK);
}

void ARPIF_delete
   (
      ARPIF_PARM_PTR  parms
   )
{
    if (parms->ihandle==NULL) {
       RTCSCMD_complete(parms, RTCS_HANDLE_ERROR);
    }
  
    if (!ARP_kill_entry(parms->ihandle, parms->PADDR)) {
      RTCSCMD_complete(parms, RTCSERR_INVALID_ADDRESS);
    }
    RTCSCMD_complete(parms, RTCS_OK);
}
#endif


#endif /* RTCSCFG_ENABLE_IP4 */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_display_if_table
*  Returned Value : 
*  Comments       :
*        Display the ARP cache for an interface.
*
*END*-----------------------------------------------------------------*/

void ARP_display_if_table(_rtcs_if_handle   ihandle)
{ /* Body */
   
   #if RTCSCFG_ENABLE_IP4
   
   IP_IF_PTR      if_ptr = (IP_IF_PTR) ihandle;
   ARP_CFG_PTR    arp_cfg ;
   ARP_ENTRY_PTR  arp_ptr;
   uint_32        i;

   if (if_ptr==NULL) return;

   arp_cfg = if_ptr->ARP;
   for (i=0;i<ARPCACHE_SIZE;i++) {
      arp_ptr = arp_cfg->CACHE[i];
      while (arp_ptr != NULL) {
         ARP_display_entry(arp_ptr);
         arp_ptr = arp_ptr->NEXT;
       }
   }
   
   #endif /* RTCSCFG_ENABLE_IP4 */
   
} /* Endbody */

/* EOF */
