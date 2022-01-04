#ifndef __rtcspcb_h__
#define __rtcspcb_h__
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
* $FileName: rtcspcb.h$
* $Version : 3.8.8.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Constants and prototype declarations which are internal
*   to the PCB module.
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/

/*
** Number of PCBs to allocate
*/
#define RTCSPCB_LAYERS_MAX  6
/* The value RTCSPCB_FRAG_MAX MUST remain 4.  If it has to be changed, you
   will need to modify the code in AH.C and ESP.C to reflect the change. */
#define RTCSPCB_FRAG_MAX    4


/*
** The maximum header size, which includes:
**    A link layer header, one of
**       Ethernet 26
**       PPP      4
**    One or more network layer headers created by RTCS (IP over IP)
**       IP       20
**    A network layer header
**       IP       60
**    An IPSEC layer
**       AH       24
**       ESP      8
**    A transport layer header, one of
**       ICMP     8+60+8 [ICMP+IP+data]
**       UDP      8
**       TCP      60
*/

#define RTCSPCB_LL_DATA_MAX         (26)
 
#if RTCSCFG_ENABLE_IPIP
   #define RTCSPCB_IPIP_DATA_MAX    (20)
#else 
   #define RTCSPCB_IPIP_DATA_MAX    (0)
#endif

#define RTCSPCB_IP_DATA_MAX         (60)

#if RTCSCFG_ENABLE_IPSEC
   #define RTCSPCB_IPSEC_DATA_MAX   (24+8)
#else 
   #define RTCSPCB_IPSEC_DATA_MAX   (0)
#endif

#if RTCSCFG_ENABLE_ICMP
   #define RTCSPCB_TL_DATA_MAX      (8+60+8)
#elif RTCSCFG_ENABLE_TCP
   #define RTCSPCB_TL_DATA_MAX      (60)
#else
   #define RTCSPCB_TL_DATA_MAX      (8)
#endif

#define RTCSPCB_DATA_MAX    (RTCSPCB_LL_DATA_MAX+RTCSPCB_IPIP_DATA_MAX+RTCSPCB_IP_DATA_MAX+RTCSPCB_IPSEC_DATA_MAX+RTCSPCB_TL_DATA_MAX)

/*
** Packet transmission types
*/
#define RTCSPCB_TYPE_UNICAST        0x01
#define RTCSPCB_TYPE_BROADCAST      0x02
#define RTCSPCB_TYPE_MULTICAST      0x04
#define RTCSPCB_TYPE_ONLYCAST       0x07


/*
** Packet Internet Protocol version number definitions
*/
#define   IPV4     0x04
#define   IPV6     0x06



/***************************************
**
** Macros
**
*/

/*
** Returns total size of PCB
*/
#define RTCSPCB_SIZE(pcb)  ((pcb)->SIZE)

/*
** Returns pointer to beginning of packet data at current layer
*/
#define RTCSPCB_DATA(pcb)           ((pcb)->CURRENT_LAYER)
#define RTCSPCB_DATA_NETWORK(pcb)   ((pcb)->NETWORK_LAYER)
#define RTCSPCB_DATA_TRANSPORT(pcb) ((pcb)->TRANSPORT_LAYER)

#define RTCSPCB_TRANSPORT_PROTL(pcb) ((pcb)->TRANS_PROTL)
#define RTCSPCB_TRANSPORT_DELTA(pcb) ((pcb)->TRANS_DELTA)

#define RTCSPCB_SET_TRANS_PROTL(pcb, protl) ((pcb)->TRANS_PROTL = (protl))
#define RTCSPCB_SET_TRANS_DELTA(pcb, delta) ((pcb)->TRANS_DELTA = (delta))


/***************************************
**
** Type definitions
**
*/

/*
** Packet Control Block
**
**  General use structure, used within all protocol layers which deal
**  with datagram-like chunks of data.  It contains packet data, where the
**  beginning of data for each layer is pointed to by LAYER[]; CL
**  indicates which entry of LAYER[] corresponds to the layer currently being
**  processed, and SIZE is the 'current' amount of data within the whole
**  packet.  Currently the layers used are ordered as follows:
**
**   layer:     0           1       2       3
**           (Medium) -\-- ARP
**                      \- IP -\-- ICMP
**                              \- UCP --- Application
**                               \ TCP --- Application
**
*/
typedef struct rtcspcb {

   struct rtcspcb _PTR_ NEXT;       /* (must be first field)            */

   PCB_PTR     PCBPTR;             /* the actual packet                */

   /*
   ** These fields are managed by the RTCSPCB_* functions
   */
   uint_32     SIZE;                /* size of total packet             */

   uchar_ptr   CURRENT_LAYER;       /* start of current header          */
   uchar_ptr   NETWORK_LAYER;       /* the network (IP) header          */
   uchar_ptr   TRANSPORT_LAYER;     /* the transport (TCP/UDP) header   */

   void (_CODE_PTR_  PCB_FREE)(PCB_PTR);  /* PCB_PTR->FREE backup       */
   pointer           PCB_PRIVATE;         /* PCB_PTR->PRIVATE backup    */
   PCB_FRAGMENT      PCB_BUFFER;          /* PCB_PTR->FRAG[0] backup    */

   PCB_PTR           PCB_ORIG;            /* PCB_PTR backup (fork)      */

   struct rtcspcb _PTR_ DUPPTR;     /* pointer to original RTCSPCB      */
   _rtcs_sem            DUPCOUNT;   /* count of duplicated packets      */





   PCB            PCB;
   PCB_FRAGMENT   FRAG[RTCSPCB_FRAG_MAX]; /* (must follow PCB)          */

   uint_32        NUM_FRAGS;        // Number of fragments in use in the above PCB. Set to 0
                                    // when PCB_PTR does not reference this PCB.

   uint_32        FREE_FRAG_BITFIELD;  /* a bit field to determine which   **
                                    ** frags must be freed or not       */



   uint_32     HEADER_FRAG_USED;    /* used bytes in first fragment     */
   uint_32     HEADER_FRAG_FREE;    /* free bytes in first fragment     */
   
   uchar       HEADER_FRAG_BUFFER[RTCSPCB_DATA_MAX];
   uchar       _RESERVED[2];        // RTCSPCB_DATA_MAX is usually not longword aligned, 
                                    // because the ethernet header is 26 bytes, so add
                                    // two bytes to keep everything else aligned.


   /*
   ** The following information must be filled before sending the
   ** PCB to IP (whether from the link layer or the transport layer)
   */

   uint_32        TYPE;             /* unicast/multicast/broadcast      */
   pointer        IFSRC;            /* source interface                 */
   uint_32 (_CODE_PTR_ IP_COMPLETE)(pointer, struct rtcspcb _PTR_ _PTR_, _ip_address, _ip_address, uint_32);

   uint_32        IP_SUM;           /* transport layer checksum         */
   uchar_ptr      IP_SUM_PTR;       /* ptr to transport layer checksum  */
   /*
   ** Used exclusively by UDP:
   **    For blocking sends, a pointer to the application parms structure
   */
   pointer        UDP_REQUEST;


   struct {
      RTCS_LINKOPT_STRUCT  RX;
      RTCS_LINKOPT_STRUCT  TX;
   }                       LINK_OPTIONS;

   uchar                   TRANS_PROTL;
   uchar                   RESERVED[3];
   uint_32                 TRANS_DELTA;
#if RTCSCFG_ENABLE_IP6
    unsigned char *next_header_p;
#endif
#if RTCSCFG_ENABLE_IPIP && RTCSCFG_ENABLE_IP4
   /*
   ** Used by IP over IP:
   **    Used in IPIP_send to send the TCPIP task a message, from TCPIP context.
   */
   _ip_address             IPIP_SOURCE;
   _ip_address             IPIP_DEST;
#endif
} RTCSPCB, _PTR_ RTCSPCB_PTR;


/***************************************
**
** Prototypes
**
*/

uint_32 RTCSPCB_init(void);
RTCSPCB_PTR RTCSPCB_alloc(void);

RTCSPCB_PTR RTCSPCB_alloc_send(void);
RTCSPCB_PTR RTCSPCB_alloc_recv(PCB_PTR);
RTCSPCB_PTR RTCSPCB_alloc_dup(RTCSPCB_PTR);
#define RTCSPCB_free(p) PCB_free((p)->PCBPTR)

uint_32 RTCSPCB_fork       (RTCSPCB_PTR);
uint_32 RTCSPCB_insert_header     (RTCSPCB_PTR, uint_32);
uint_32 RTCSPCB_next       (RTCSPCB_PTR, uint_32);
uint_32 RTCSPCB_shrink     (RTCSPCB_PTR, uint_32);
uint_32 RTCSPCB_append_fragment       (RTCSPCB_PTR, uint_32, uchar_ptr);
uint_32 RTCSPCB_append_fragment_autofree (RTCSPCB_PTR, uint_32, uchar_ptr);
uint_32 RTCSPCB_depend     (RTCSPCB_PTR, RTCSPCB_PTR);

void RTCSPCB_memcopy (RTCSPCB_PTR, pointer, uint_32, uint_32);

#endif
/* EOF */
