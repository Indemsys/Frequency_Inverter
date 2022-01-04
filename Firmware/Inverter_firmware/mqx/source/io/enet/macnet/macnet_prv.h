/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2009 Embedded Access Inc.;
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
* $FileName: macnet_prv.h$
* $Version : 3.8.18.1$
* $Date    : Oct-8-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the ethernet drivers for the ColdFire 32-bit MACNET 
*
*END************************************************************************/
#ifndef _macnet_prv_h
#define _macnet_prv_h 1

#include "macnet_1588.h"
#include "lwevent.h"

#define MACNET_RESET_TIMEOUT       (0x10000)

#define MACNET_RX_ALIGN(n) ENET_ALIGN(n,MACNET_RX_BUFFER_ALIGNMENT)
#define MACNET_TX_ALIGN(n) ENET_ALIGN(n,MACNET_TX_BUFFER_ALIGNMENT)
#define MACNET_BD_ALIGN(n) ENET_ALIGN(n,MACNET_BD_ALIGNMENT)

#define MACNET_NUM_ISRS      5

/* Assumes RX_BUFFER_SIZE >= 256, so that 256*6=1536, set higher for smaller RX_BUFFER_SIZE */
#define MACNET_MAX_FRAGS     6     

#define MACNET_int_disable()       _int_disable()
#define MACNET_int_enable()        _int_enable()
                                 


/* Set ENETCFG_SUPPORT_FRAGMENTED_PCBS to 1 if the upper layer can support receive PCBs with multiple
 fragments.  NOTE: RTCS currently does not support this.*/
#ifndef ENETCFG_SUPPORT_FRAGMENTED_PCBS
   #define ENETCFG_SUPPORT_FRAGMENTED_PCBS 0
#endif

/* Allow timestamping functionality */
#ifndef ENETCFG_SUPPORT_PTP
   #define ENETCFG_SUPPORT_PTP 0
#endif

#define BD_INC(index,maxnum)   if (++index == maxnum) index = 0


#ifdef __cplusplus
extern "C" {
#endif

#if PSP_MQX_CPU_IS_VYBRID
#define ENET_INT_RX_TX        0       /* Vybrid MACNET interrupt */ 
#define ENET_NUM_INTS         1   
#else
#define ENET_INT_TX_INTF      0       /* Transmit frame interrupt */
#define ENET_INT_TX_INTB      1       /* Transmit buffer interrupt */
#define ENET_INT_TX_UN        2       /* Transmit FIFO underrun */
#define ENET_INT_RL           3       /* Collision retry limit */
#define ENET_INT_RX_INTF      4       /* Receive frame interrupt */
#define ENET_INT_RX_INTB      5       /* Receive buffer interrupt */
#define ENET_INT_MII          6       /* MII interrupt */
#define ENET_INT_LC           7       /* Late collision */
#define ENET_INT_GRA          8       /* Graceful stop complete */
#define ENET_INT_EBERR        9       /* Ethernet bus error */
#define ENET_INT_BABT        10       /* Babbling transmit error */
#define ENET_INT_BABR        11       /* Babbling receive error */
#define ENET_INT_TS_AVAIL    12       /* Timestamp available */
#define ENET_INT_WAKEUP      13       /* Wake from sleep */
#define ENET_INT_PLR         14       /* Payload receive error */
#define ENET_INT_TS_TIMER    15       /* TS timer overflow */
#define ENET_NUM_INTS        16      
#endif //END PSP_MQX_CPU_IS_VYBRID

/* Ethernet Buffer descriptor bits */
#define ENET_BD_ETHER_RX_EMPTY                (1<<15)
#define ENET_BD_ETHER_RX_SOFTWARE_OWNERSHIP_1 (1<<14)
#define ENET_BD_ETHER_RX_WRAP                 (1<<13)
#define ENET_BD_ETHER_RX_SOFTWARE_OWNERSHIP_2 (1<<12)
#define ENET_BD_ETHER_RX_LAST_FRAME           (1<<11)
#define ENET_BD_ETHER_RX_MISS                 (1<<8)
#define ENET_BD_ETHER_RX_BROADCAST            (1<<7)
#define ENET_BD_ETHER_RX_MULTICAST            (1<<6)
#define ENET_BD_ETHER_RX_LENGTH_VIOLATION     (1<<5)
#define ENET_BD_ETHER_RX_NON_OCTET            (1<<4)
#define ENET_BD_ETHER_RX_CRC_ERROR            (1<<2)
#define ENET_BD_ETHER_RX_OVERRUN              (1<<1)
#define ENET_BD_ETHER_RX_TRUNCATED            (1<<0)

#define ENET_BD_EXT0_ETHER_RX_MAC_ERR         (1<<15)
#define ENET_BD_EXT0_ETHER_RX_PHY_ERR         (1<<10)
#define ENET_BD_EXT0_ETHER_RX_COLLISION       (1<<9)
#define ENET_BD_EXT0_ETHER_RX_UNICAST         (1<<8)
#define ENET_BD_EXT0_ETHER_RX_GENERATE_INTR   (1<<7)

#define ENET_BD_EXT1_ETHER_RX_IPHEADER_CHSUM_ERR  (1<<5)
#define ENET_BD_EXT1_ETHER_RX_PROTOCOL_CHSUM_ERR  (1<<4)
#define ENET_BD_EXT1_ETHER_RX_VLAN                (1<<2)
#define ENET_BD_EXT1_ETHER_RX_IPV6FRAME           (1<<1)
#define ENET_BD_EXT1_ETHER_RX_IPV4FRAGMENT        (1<<0)

#define ENET_BD_EXT2_ETHER_RX_LAST_BD_UPDATE_DONE 		(1<<15)

#define ENET_BD_ETHER_TX_READY                			(1<<15)
#define ENET_BD_ETHER_TX_SOFTWARE_OWNERSHIP_1 			(1<<14)
#define ENET_BD_ETHER_TX_WRAP                 			(1<<13)
#define ENET_BD_ETHER_TX_SOFTWARE_OWNERSHIP_2 			(1<<12)
#define ENET_BD_ETHER_TX_LAST                 			(1<<11)
#define ENET_BD_ETHER_TX_SEND_CRC             			(1<<10)

#define ENET_BD_EXT0_ETHER_TX_GENERATE_INTR          	(1<<14)
#define ENET_BD_EXT0_ETHER_TX_TIMESTAMP              	(1<<13)
#define ENET_BD_EXT0_ETHER_TX_INSERT_PROT_SPEC_CHSUM 	(1<<12)
#define ENET_BD_EXT0_ETHER_TX_INSERT_IPHEADER_CHSUM  	(1<<11)

#define ENET_BD_EXT1_ETHER_TX_ERR                  		(1<<15)
#define ENET_BD_EXT1_ETHER_TX_UNDERFLOW_ERR        		(1<<13)
#define ENET_BD_EXT1_ETHER_TX_EXCESS_COLLISION_ERR 		(1<<12)
#define ENET_BD_EXT1_ETHER_TX_FRAME_WIDTH_ERR      		(1<<11)
#define ENET_BD_EXT1_ETHER_TX_LATE_COLLISION_ERR   		(1<<10)
#define ENET_BD_EXT1_ETHER_TX_LATE_OVERFLOW_ERR    		(1<<9)
#define ENET_BD_EXT1_ETHER_TX_LATE_TIMESTAMP_ERR   		(1<<8)

#define ENET_BD_EXT2_ETHER_TX_LAST_BD_UPDATE_DONE  		(1<<15)

#define ENET_EIR_ALL_PENDING 							(0x7FFF8000)
	
typedef uint_16 bd_index_t;


#if ENETCFG_SUPPORT_FRAGMENTED_PCBS

   typedef struct macnet_rx_pcb
   {
      PCB            _FRAG0;
      PCB_FRAGMENT   _FRAGS[MACNET_MAX_FRAGS];
      PCB_FRAGMENT   _FREE_FRAG;
   } MACNET_RX_PCB, _PTR_ MACNET_RX_PCB_PTR;
#else
   typedef PCB2 MACNET_RX_PCB, _PTR_ MACNET_RX_PCB_PTR;
#endif



/* The Ethernet initialization structure */
typedef struct {
   uint_32      BUS_CLOCK;
   _mqx_uint    ERX_LEVEL;      /* Receive interrupt level */
   _mqx_uint    ERX_SUBLEVEL;   /* Receive interrupt sublevel */
   _mqx_uint    ETX_LEVEL;      /* Transmit interrupt level */
   _mqx_uint	ETX_SUBLEVEL;   /* Transmit interrupt sublevel */
} MACNET_INIT_STRUCT, _PTR_ MACNET_INIT_STRUCT_PTR;


#define MACNET_PTP_DEFAULT_RX_BUF_SZ      25
#define MACNET_PTP_DEFAULT_L2PCK_BUF_SZ   10

#define MACNET_PTP_MSG_SYNC              0x0
#define MACNET_PTP_MSG_DEL_REQ           0x1
#define MACNET_PTP_MSG_P_DEL_REQ         0x2
#define MACNET_PTP_MSG_P_DEL_RESP        0x3
#define MACNET_PTP_MSG_DEL_RESP          0x4
#define MACNET_PTP_MSG_ALL_OTHER         0x5

#define MACNET_PTP_UDP_IPLENGHT_OFFS     0x10
#define MACNET_PTP_UDP_PKT_TYPE_OFFS     0x17
#define MACNET_PTP_UDP_IPCHECKSUM_OFFS   0x18
#define MACNET_PTP_UDP_MULTI_ADDR_OFFS   0x1E
#define MACNET_PTP_UDP_PORT_OFFS         0x24
#define MACNET_PTP_UDP_UDPLENGHT_OFFS    0x26
#define MACNET_PTP_UDP_UDPCHECKSUM_OFFS  0x28
#define MACNET_PTP_UDP_MSG_TYPE_OFFS     0x2A
#define MACNET_PTP_UDP_CLOCKID           0x3E
#define MACNET_PTP_UDP_SEQ_ID_OFFS       0x48
#define MACNET_PTP_UDP_CTRL_OFFS         0x4A

#define MACNET_PTP_ETHER_PKT_TYPE_OFFS   0x0C
#define MACNET_PTP_ETHER_MSG_TYPE_OFFS   0x0E
#define MACNET_PTP_ETHER_CLOCKID         0x22
#define MACNET_PTP_ETHER_SEQ_ID_OFFS     0x2C
#define MACNET_PTP_ETHER_CTRL_OFFS       0x2E

#define MACNET_PACKET_TYPE_UDP           0x11
#define MACNET_PACKET_TYPE_IEEE_802_3    0x88F7
#define MACNET_PTP_EVNT_PORT             (319)
#define MACNET_PTP_GNRL_PORT             (320)

#define MACNET_PTP_LWEVENT_RX_TS         0x01
#define MACNET_PTP_LWEVENT_TX_TS         0x02
#define MACNET_PTP_LWEVENT_TX_TS_INTR    0x04

#define MACNET_PTP_COMM_PROTO_UDP_IPv4   0x01
#define MACNET_PTP_COMM_PROTO_UDP_IPv6   0x02
#define MACNET_PTP_COMM_PROTO_IEEE_802_3 0x03

#define MACNET_PTP_CLOCKID_SIZE          0x08


/* Structure for PTP Time Stamp */
typedef struct {
	uint_32		    KEY;
	uint_8          CLOCKID[10];
	MACNET_PTP_TIME	TS_TIME;
} MACNET_PTP_DATA;

typedef struct {
	uint_32	        FRONT;
	uint_32	        END;
	uint_32	        SIZE;
	MACNET_PTP_DATA *DATA_BUF;
} MACNET_PTP_CIRCULAR,  _PTR_ MACNET_PTP_CIRCULAR_PTR;

typedef struct 
{
  uint_8  PACKET[ENET_FRAMESIZE_MAXDATA];
  uint_16 LENGTH;
} MACNET_PTP_L2PKT;

typedef struct 
{
  /* Packet queue array */
  MACNET_PTP_L2PKT L2PCK[MACNET_PTP_DEFAULT_L2PCK_BUF_SZ];
  /* Queue write index */ 
  uint_32 WR_IDX;
  /* Queue read index */ 
  uint_32 RD_IDX;
} MACNET_PTP_L2QUEUE,  _PTR_ MACNET_PTP_L2QUEUE_PTR;

typedef struct {	
	ENET_MemMapPtr            MACNET_PTR;
	MACNET_PTP_CIRCULAR       RX_TIME_SYNC;
	MACNET_PTP_CIRCULAR       RX_TIME_DEL_REQ;
	MACNET_PTP_CIRCULAR       RX_TIME_PDEL_REQ;
	MACNET_PTP_CIRCULAR       RX_TIME_PDEL_RESP;
	MACNET_PTP_CIRCULAR       TX_TIME;
	LWEVENT_STRUCT            LWEVENT_PTP;
    uint_32                   PTIMER_PRESENT;
	MACNET_PTP_TIME           TXSTAMP;
	MACNET_PTP_L2QUEUE_PTR    L2PCKS_PTR;
} MACNET_PTP_PRIVATE,  _PTR_ MACNET_PTP_PRIVATE_PTR;


/*
** ENET_BD_STRUCT
** This structure defines what the Ethernet buffer descriptor looks like
*/
typedef struct enet_bd_struct
{
   uint_16   CONTROL;
   uint_16   LENGTH;
   uchar_ptr BUFFER;
   uint_16   CONTROL_EXT0;
   uint_16   CONTROL_EXT1;
   uint_8    HEADER_LENGTH;
   uint_8    PROTOCOL_TYPE;
   uint_16   PAYLOAD_CHECKSUM;
   uint_16   CONTROL_EXT2;
   uint_16   reserved0;
   uchar_ptr TIMESTAMP;
   uint_16   reserved1;
   uint_16   reserved2;
   uint_16   reserved3;
   uint_16   reserved4;
} ENET_BD_STRUCT, _PTR_ ENET_BD_STRUCT_PTR;
typedef volatile struct enet_bd_struct _PTR_ VENET_BD_STRUCT_PTR;



typedef struct mcf_macnet_context_struct {
    ENET_MemMapPtr        MACNET_ADDRESS;
    ENET_MemMapPtr        PHY_PTR;
    uint_16               AlignedRxBufferSize;
    uint_16               AlignedTxBufferSize;
   
   /*
   **    The Receive-Side State
   **
   ** RxPCBHead and RxPCBTail are the head and tail of a linked list
   ** of available PCBs for receiving packets.  RxEntries is
   ** the length of this list.
   **
   ** NextRxBD points to the 'first' queued receive descriptor, i.e., usually
   ** the one that RBPTR is pointing to.  When an RXF interrupt occurs,
   ** NextRxBD is pointing to the first descriptor with a received packet.
   ** The packets is forwarded to the upper layers, and NextRxBD is incremented.
   **
   ** LastRxBD points to the 'last' queued receive descriptor, i.e., the last
   ** one with E=1.  The LastRxBD descriptor usually precedes the LastRxBD descriptor.
   ** When the upper layers free a received packet, it gets placed at LastRxBD
   ** (with E=1), and LastRxBD is incremented.
   */

   PCB_PTR                       RxPCBHead;
   PCB_PTR                       RxPCBTail;
   
   uint_32                       CurrentRxFrag;
   ENET_ECB_STRUCT_PTR           CurrentRxECB;
#if ENETCFG_SUPPORT_FRAGMENTED_PCBS
   PCB_PTR                       CurrentRxPCB;     /* The PCB that is currently being built */
   boolean                       RxDiscarding;
#else
   PCB_FRAGMENT                  FRAGS[MACNET_MAX_FRAGS];
#endif


   bd_index_t                    ActiveRxBDs;
   bd_index_t                    NextRxBD;
   bd_index_t                    LastRxBD;
   bd_index_t                    NumRxBDs;

   /*
   **    The Transmit-Side State
   **
   ** TxPCBS_PTR is an array of pointers to the PCBs queued on the transmit ring.
   ** AvailableTxBDs is the number of free entries on the ring.
   **
   ** NextTxBD points to the first available transmit descriptor (when AvailableTxBDs>0),
   ** or to LastTxBD (when AvailableTxBDs==NumTxBDs).  It is the descriptor that will next
   ** be queued onto the transmit ring, as soon as there's a packet to send
   ** and room on the transmit ring.
   **
   ** LastTxBD points to the 'last' queued transmit descriptor, i.e., the one
   ** that TBPTR is pointing to.  When a TXB interrupt occurs, LastTxBD is
   ** pointing to the transmitted buffer.  The buffer is freed, and LastTxBD is
   ** incremented.
   **
   ** TxErrors keeps track of all the errors that occur during transmission
   ** of a frame comprised of multiple buffers.
   */

   PCB_PTR *                     TxPCBS_PTR;
   bd_index_t                    AvailableTxBDs;
   bd_index_t                    NextTxBD;
   bd_index_t                    LastTxBD;
   bd_index_t                    NumTxBDs;
   uint_32                       TxErrors;



   MACNET_RX_PCB_PTR             RX_PCB_BASE;
   pointer                       UNALIGNED_RING_PTR;        
   VENET_BD_STRUCT_PTR           MACNET_RX_RING_PTR; 
   VENET_BD_STRUCT_PTR           MACNET_TX_RING_PTR; 
   pointer                       UNALIGNED_BUFFERS;  
   uchar_ptr                     RX_BUFFERS;  
   uchar_ptr                     TX_BUFFERS;  
   uchar_ptr                     SMALL_BUFFERS;  
   uchar_ptr                     LARGE_BUFFERS;  
   uint_32                       FREE_TX;
   uint_32                       FREE_TX_SMALL;
   
#if BSPCFG_ENET_RESTORE
   /* These fields are kept only for ENET_shutdown() */
   INT_ISR_FPTR                  OLDISR_PTR[MACNET_NUM_ISRS];
   pointer                       OLDISR_DATA[MACNET_NUM_ISRS];
#endif
      
#if ENETCFG_SUPPORT_PTP
   MACNET_PTP_PRIVATE_PTR        PTP_PRIV;
#endif
} MACNET_CONTEXT_STRUCT, * MACNET_CONTEXT_STRUCT_PTR;


extern const   MACNET_INIT_STRUCT MACNET_device[];
extern const   ENET_MAC_IF_STRUCT MACNET_IF;
extern const   uint_32 MACNET_vectors[MACNET_DEVICE_COUNT][ENET_NUM_INTS];

extern boolean MACNET_install_isrs( ENET_CONTEXT_STRUCT_PTR enet_ptr, MACNET_INIT_STRUCT const * enet_init_ptr  ); 
extern boolean MACNET_install_isr( ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 int_num, uint_32 int_index, 
                                   INT_ISR_FPTR isr, uint_32 level, uint_32 sublevel  );
extern void    MACNET_add_buffers_to_rx_ring(MACNET_CONTEXT_STRUCT_PTR macnet_context_ptr);
extern uint_32 MACNET_initialize(ENET_CONTEXT_STRUCT_PTR);
extern uint_32 MACNET_shutdown( ENET_CONTEXT_STRUCT_PTR );
extern uint_32 MACNET_send( ENET_CONTEXT_STRUCT_PTR, PCB_PTR packet, uint_32 size, uint_32 frags, uint_32 flags );
extern uint_32 MACNET_join( ENET_CONTEXT_STRUCT_PTR, ENET_MCB_STRUCT_PTR mcb_ptr );
extern uint_32 MACNET_rejoin( ENET_CONTEXT_STRUCT_PTR );
extern void    MACNET_mask_interrupts( ENET_CONTEXT_STRUCT_PTR enet_ptr ); 
extern void    MACNET_uninstall_all_isrs( ENET_CONTEXT_STRUCT_PTR enet_ptr );
extern void    MACNET_TX_ISR(pointer);
extern void    MACNET_RX_ISR(pointer);
extern pointer MACNET_get_base_address (uint_32);
extern uint_32 MACNET_get_vector(uint_32,uint_32);
extern void    MACNET_io_init(uint_32);
extern void    MACNET_free_context( MACNET_CONTEXT_STRUCT_PTR macnet_context_ptr );

#if ENETCFG_SUPPORT_PTP
extern boolean MACNET_ptp_install_ts_avail_isr( ENET_CONTEXT_STRUCT_PTR enet_ptr, MACNET_INIT_STRUCT const * enet_init_ptr ); 
extern boolean MACNET_ptp_install_ts_timer_isr( ENET_CONTEXT_STRUCT_PTR enet_ptr, MACNET_INIT_STRUCT const * enet_init_ptr ); 
extern uint_32 MACNET_ptp_init(ENET_CONTEXT_STRUCT_PTR enet_ptr);
extern void    MACNET_ptp_store_txstamp(pointer);
extern void    MACNET_ptp_store_rxstamp(ENET_CONTEXT_STRUCT_PTR enet_ptr, PCB_PTR pcb_ptr, VENET_BD_STRUCT_PTR bdp);
extern void    MACNET_ptp_increment_seconds(pointer);
extern uint_32 MACNET_ptp_start(MACNET_PTP_PRIVATE *priv, boolean master_mode);
extern void    MACNET_ptp_stop(ENET_MemMapPtr macnet_ptr);
extern void    MACNET_ptp_cleanup(ENET_CONTEXT_STRUCT_PTR enet_ptr);
extern boolean MACNET_ptp_insert(MACNET_PTP_CIRCULAR *buf, MACNET_PTP_DATA *data);
extern ENET_MemMapPtr MACNET_ptp_get_master_base_address(void);
extern void    MACNET_ptp_set_master_base_address(ENET_MemMapPtr addr);
extern int_32  MACNET_ptp_get_rx_timestamp(MACNET_PTP_PRIVATE *priv, pointer param_ptr, MACNET_PTP_TIME *rx_time);
extern int_32  MACNET_ptp_get_tx_timestamp(MACNET_PTP_PRIVATE *priv, pointer param_ptr, MACNET_PTP_TIME *tx_time);
extern void    MACNET_ptp_service_L2pckts(PCB_PTR  pcb, pointer handle);
extern int_32  MACNET_ptp_send_L2pckts(_enet_handle handle, pointer param_ptr);
extern int_32  MACNET_ptp_recv_L2pckts(_enet_handle handle, pointer param_ptr);
extern uint_32 MACNET_ptp_ioctl(ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 command_id, pointer inout_param);
#endif /* ENETCFG_SUPPORT_PTP */

boolean MACNET_read_mii(ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 reg_index, uint_32_ptr data, uint_32 timeout);
boolean MACNET_write_mii(ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 reg_index, uint_32 data, uint_32 timeout);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

