/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: macnet_init.c$
* $Version : 3.8.15.0$
* $Date    : Aug-7-2012$
*
* Comments:
*
*   This file contains the MACNET initialization
*
*END************************************************************************/

#include "mqx.h"

#include "mqx_inc.h"

#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "bsp_prv.h"
#include "macnet_prv.h"          

extern _mem_pool_id _BSP_sram_pool;

const ENET_MAC_IF_STRUCT MACNET_IF = {
    MACNET_initialize,
    MACNET_shutdown,
    MACNET_send,
    MACNET_read_mii,
    MACNET_write_mii,

    #if BSPCFG_ENABLE_ENET_MULTICAST
    MACNET_join,
    MACNET_rejoin,
    #endif /* BSPCFG_ENABLE_ENET_MULTICAST */

    #if ENETCFG_SUPPORT_PTP
    MACNET_ptp_ioctl,
    #else
    NULL,
    #endif /* ENETCFG_SUPPORT_PTP */
};


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_initialize
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Initializes the chip (extended version).
*
*END*-----------------------------------------------------------------*/

#define IF_ERROR_EXIT(c,x) {if (c) { error = x; goto EXIT; }}

uint_32 MACNET_initialize
   (
      ENET_CONTEXT_STRUCT_PTR enet_ptr
   )
{ 
   MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr=NULL;
   ENET_MemMapPtr             macnet_ptr;
   VENET_BD_STRUCT_PTR        bd_ptr;
   uchar_ptr                  buf_ptr;
   MACNET_RX_PCB_PTR          pcb_ptr;
   uint_32                    i, rxsize, txsize, ssize, lsize, pcbsize,rx_pcb_size, large_packet_size, rcr;
   uint_32                    timeout, error = ENET_OK;
   boolean                    bOK;
#if ENETCFG_SUPPORT_PTP
   MACNET_PTP_PRIVATE_PTR     macnet_ptp_ptr = NULL;
#endif /* ENETCFG_SUPPORT_PTP */
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   
   _GET_KERNEL_DATA(kernel_data);

   // Initialize the MACNET I/O Pins
   MACNET_io_init(enet_ptr->PARAM_PTR->ENET_IF->MAC_NUMBER);
   
   macnet_ptr = MACNET_get_base_address(enet_ptr->PARAM_PTR->ENET_IF->MAC_NUMBER);
   if (macnet_ptr == NULL) {
      return ENETERR_INVALID_DEVICE;
   }
   
   
   // currently limit number of TX BDs to 32, as a bitmask is used in the free function.
   if (enet_ptr->PARAM_PTR->NUM_TX_ENTRIES > 32) {
      return ENETERR_INVALID_INIT_PARAM;
   }
   
   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   MACNET_int_disable();

   macnet_context_ptr = _mem_alloc_system_zero(sizeof(MACNET_CONTEXT_STRUCT));
   IF_ERROR_EXIT((NULL==macnet_context_ptr), ENETERR_ALLOC_MAC_CONTEXT);
   _mem_set_type((pointer)macnet_context_ptr, MEM_TYPE_IO_ENET_MAC_CONTEXT_STRUCT);
   
   enet_ptr->MAC_CONTEXT_PTR = (pointer) macnet_context_ptr;  

   macnet_context_ptr->MACNET_ADDRESS = macnet_ptr;
   macnet_context_ptr->PHY_PTR = MACNET_get_base_address(enet_ptr->PARAM_PTR->ENET_IF->PHY_NUMBER);

   /* Stop the chip */
   macnet_ptr->ECR = ENET_ECR_RESET_MASK;

   /* wait until the initialization cycle completes */
   timeout = 0;
   while ((macnet_ptr->ECR & ENET_ECR_RESET_MASK) && (timeout<MACNET_RESET_TIMEOUT)){
      _time_delay(1);
      timeout++;
   } 
   IF_ERROR_EXIT((macnet_ptr->ECR & ENET_ECR_RESET_MASK), ENETERR_INIT_FAILED);
   
   /* Disable all MACNET interrupts */
   macnet_ptr->EIMR = 0;

   /* clear any pending interrpts */
   macnet_ptr->EIR = ENET_EIR_ALL_PENDING;

   macnet_context_ptr->NumRxBDs = enet_ptr->PARAM_PTR->NUM_RX_ENTRIES;
   macnet_context_ptr->NumTxBDs = enet_ptr->PARAM_PTR->NUM_TX_ENTRIES;
     
   // Compute aligned buffer sizes 
   if (enet_ptr->PARAM_PTR->TX_BUFFER_SIZE) {
      macnet_context_ptr->AlignedTxBufferSize = MACNET_TX_ALIGN(enet_ptr->PARAM_PTR->TX_BUFFER_SIZE);
   } else {
      macnet_context_ptr->AlignedTxBufferSize = MACNET_TX_ALIGN(enet_ptr->MaxTxFrameSize);
   }

   if (enet_ptr->PARAM_PTR->RX_BUFFER_SIZE) {
      macnet_context_ptr->AlignedRxBufferSize = MACNET_RX_ALIGN(enet_ptr->PARAM_PTR->RX_BUFFER_SIZE);
   } else {
      macnet_context_ptr->AlignedRxBufferSize = MACNET_RX_ALIGN(enet_ptr->MaxRxFrameSize);
   }

   // Allocate the Transmit and Receive buffer descriptors
   // TODO remake to using alloc_align fn
#if BSPCFG_HAS_SRAM_POOL && BSPCFG_ENET_SRAM_BUF
   bd_ptr = (VENET_BD_STRUCT_PTR)_mem_alloc_system_zero_from(_BSP_sram_pool, (sizeof(ENET_BD_STRUCT)*(macnet_context_ptr->NumRxBDs+macnet_context_ptr->NumTxBDs))+MACNET_BD_ALIGNMENT);
#else
   bd_ptr = (VENET_BD_STRUCT_PTR)_mem_alloc_system_zero_uncached((sizeof(ENET_BD_STRUCT)*(macnet_context_ptr->NumRxBDs+macnet_context_ptr->NumTxBDs))+MACNET_BD_ALIGNMENT);
#endif

   IF_ERROR_EXIT((NULL == bd_ptr), ENETERR_ALLOC_BD);
   _mem_set_type((pointer)bd_ptr, MEM_TYPE_IO_BD_STRUCT);

   macnet_context_ptr->UNALIGNED_RING_PTR = (pointer) bd_ptr;
   macnet_context_ptr->MACNET_RX_RING_PTR = (VENET_BD_STRUCT_PTR) MACNET_BD_ALIGN((uint_32)bd_ptr);
   macnet_context_ptr->MACNET_TX_RING_PTR = &macnet_context_ptr->MACNET_RX_RING_PTR[macnet_context_ptr->NumRxBDs];

   /* Set wrap bit in last BD */
   macnet_context_ptr->MACNET_RX_RING_PTR[macnet_context_ptr->NumRxBDs - 1].CONTROL = HOST_TO_BE_SHORT_CONST(ENET_BD_ETHER_RX_WRAP);
   macnet_context_ptr->MACNET_TX_RING_PTR[macnet_context_ptr->NumTxBDs - 1].CONTROL = HOST_TO_BE_SHORT_CONST(ENET_BD_ETHER_TX_WRAP);

   macnet_context_ptr->AvailableTxBDs = macnet_context_ptr->NumTxBDs;

   // Allocate array to hold Transmit PCB pointers while they are queued for transmission
   macnet_context_ptr->TxPCBS_PTR = (PCB_PTR *) _mem_alloc_system_zero(sizeof(PCB_PTR)*macnet_context_ptr->NumTxBDs);
   IF_ERROR_EXIT((NULL==macnet_context_ptr->TxPCBS_PTR), ENETERR_ALLOC_PCB);
   _mem_set_type((pointer)macnet_context_ptr->TxPCBS_PTR, MEM_TYPE_IO_PCB_PTR);

   // Allocate the Receive PCBs
   rx_pcb_size = sizeof(MACNET_RX_PCB);
   pcbsize = enet_ptr->PARAM_PTR->NUM_RX_PCBS * rx_pcb_size;
   macnet_context_ptr->RX_PCB_BASE = (MACNET_RX_PCB_PTR) _mem_alloc_system_zero(pcbsize);
   IF_ERROR_EXIT((NULL==macnet_context_ptr->RX_PCB_BASE), ENETERR_ALLOC_PCB);
   _mem_set_type((pointer)macnet_context_ptr->RX_PCB_BASE, MEM_TYPE_IO_PCB_STRUCT);
       
   // Allocate the Transmit and Receive buffers
   txsize = (enet_ptr->PARAM_PTR->NUM_TX_BUFFERS * macnet_context_ptr->AlignedTxBufferSize) + MACNET_TX_BUFFER_ALIGNMENT;
   rxsize = (enet_ptr->PARAM_PTR->NUM_RX_BUFFERS  * macnet_context_ptr->AlignedRxBufferSize) +MACNET_RX_BUFFER_ALIGNMENT;
   ssize  = (enet_ptr->PARAM_PTR->NUM_SMALL_BUFFERS * MACNET_SMALL_PACKET_SIZE);
   large_packet_size = enet_ptr->PARAM_PTR->OPTIONS&ENET_OPTION_VLAN ? ENET_FRAMESIZE_VLAN : ENET_FRAMESIZE;
   lsize  = (enet_ptr->PARAM_PTR->NUM_LARGE_BUFFERS * large_packet_size);

#if BSPCFG_HAS_SRAM_POOL && BSPCFG_ENET_SRAM_BUF
   buf_ptr = _mem_alloc_system_from(_BSP_sram_pool, rxsize + txsize + ssize + lsize);
#else
   buf_ptr = _mem_alloc_system_uncached(rxsize + txsize + ssize + lsize);  // temporary fix for cache problems... previously _mem_alloc_system
#endif
   
   IF_ERROR_EXIT((NULL==buf_ptr), ENETERR_ALLOC_BUFFERS);
   _mem_set_type(buf_ptr, MEM_TYPE_IO_ENET_BUFFERS);

   macnet_context_ptr->UNALIGNED_BUFFERS = buf_ptr;

   // Align to TX buffer boundary
   buf_ptr = (uchar_ptr)MACNET_TX_ALIGN((uint_32)buf_ptr);
   
   // Queue packets on TX Buffer Q.
   macnet_context_ptr->TX_BUFFERS = NULL;
   for (i=0;i<enet_ptr->PARAM_PTR->NUM_TX_BUFFERS;i++) {
      ENET_Enqueue_Buffer((pointer *)&macnet_context_ptr->TX_BUFFERS, buf_ptr);
      buf_ptr += macnet_context_ptr->AlignedTxBufferSize;
   }
   
   // Align to RX buffer boundary
   buf_ptr = (uchar_ptr)MACNET_RX_ALIGN((uint_32)buf_ptr);
  
   // Queue packets on RX Buffer Q.
   macnet_context_ptr->RX_BUFFERS = NULL;
   for (i=0;i<enet_ptr->PARAM_PTR->NUM_RX_BUFFERS;i++) {
      ENET_Enqueue_Buffer((pointer *)&macnet_context_ptr->RX_BUFFERS, buf_ptr);
      buf_ptr += macnet_context_ptr->AlignedRxBufferSize;
   }

   // Queue small packets on small buffer Q.
   for (i=0;i<enet_ptr->PARAM_PTR->NUM_SMALL_BUFFERS;i++) {
      ENET_Enqueue_Buffer((pointer *)&macnet_context_ptr->SMALL_BUFFERS, buf_ptr);
      buf_ptr += MACNET_SMALL_PACKET_SIZE;
   }

   // Queue large packets on large buffer Q.
   for (i=0;i<enet_ptr->PARAM_PTR->NUM_LARGE_BUFFERS;i++) {
      ENET_Enqueue_Buffer((pointer *)&macnet_context_ptr->LARGE_BUFFERS, buf_ptr);
      buf_ptr += large_packet_size;
   }

   // Enqueue the RX PCBs onto the receive PCB queue 
   pcb_ptr = macnet_context_ptr->RX_PCB_BASE;
   for (i = 0; i < enet_ptr->PARAM_PTR->NUM_RX_PCBS; i++) {
      QADD(macnet_context_ptr->RxPCBHead, macnet_context_ptr->RxPCBTail, (PCB_PTR) pcb_ptr);
      pcb_ptr++;
   } 

   // Fill up the receive ring 
   MACNET_add_buffers_to_rx_ring(macnet_context_ptr);

   /* Program this station's Ethernet physical address */
   macnet_ptr->PALR = (uint_32)((enet_ptr->ADDRESS[0] << 24)|(enet_ptr->ADDRESS[1] << 16)|(enet_ptr->ADDRESS[2] << 8)|(enet_ptr->ADDRESS[3] << 0));
   macnet_ptr->PAUR = (uint_32)((enet_ptr->ADDRESS[4] << 24)|(enet_ptr->ADDRESS[5] << 16));
   // Clear the individual hash table registers
   macnet_ptr->IAUR = 0;
   macnet_ptr->IALR = 0;

   // Clear the group hash table registers                                
   macnet_ptr->GAUR = 0;
   macnet_ptr->GALR = 0;  

   /* Program receive buffer size */
   macnet_ptr->MRBR = macnet_context_ptr->AlignedRxBufferSize;     

   // Configure start of Rx and Tx BD rings
   macnet_ptr->RDSR = (uint_32)(macnet_context_ptr->MACNET_RX_RING_PTR);
   macnet_ptr->TDSR = (uint_32)(macnet_context_ptr->MACNET_TX_RING_PTR);

   // Set Receive Frame size 
   // NOTE: Oddly, the Receive Control Register (RCR) afmacnetts the transmit side too.  The RCR is used to determine if the 
   //       transmitter is babbling, which means, if the RX buffer size < Tx Buffer size, we can get babling transmitter
   //       errors if we set RCR to the maximum Receive frame length.  We really have no choice but to set RCR to one
   //       of ENET_FRAMESIZE or ENET_FRAMESIZE_VLAN.
   //

   rcr = ENET_RCR_MII_MODE_MASK | (ENET_RCR_MAX_FL_MASK & (large_packet_size << ENET_RCR_MAX_FL_SHIFT));
   if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_RMII) {
      rcr |= ENET_RCR_RMII_MODE_MASK;
   } else if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_7WIRE) {
      rcr &= ~ENET_RCR_MII_MODE_MASK;
   }
   macnet_ptr->RCR =  rcr;
   

   if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_MAC_LOOPBACK) {
      macnet_ptr->RCR |= ENET_RCR_LOOP_MASK;
   }
   //  Set Full/Half duplex based on mode.
   if (enet_ptr->PARAM_PTR->MODE & ENET_HALF_DUPLEX) {
      macnet_ptr->TCR = 0; // half duplex
   } else {
      macnet_ptr->TCR = 4; // full duplex
   }
   
   // Enable MII_SPEED register 
   i = (MACNET_device[enet_ptr->PARAM_PTR->ENET_IF->MAC_NUMBER].BUS_CLOCK / enet_ptr->PARAM_PTR->ENET_IF->PHY_MII_SPEED + 1) & ~1;
   if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_NO_PREAMBLE) {
      i |= ENET_MSCR_DIS_PRE_MASK;
   }
   macnet_context_ptr->PHY_PTR->MSCR = i;

   // Zero counters
   macnet_ptr->MIBC |= ENET_MIBC_MIB_CLEAR_MASK;

   // Install the ISRs
   bOK = MACNET_install_isrs( enet_ptr, &MACNET_device[enet_ptr->PARAM_PTR->ENET_IF->MAC_NUMBER] );
   IF_ERROR_EXIT(!bOK, ENETERR_INSTALL_ISR);
          
   // Unmask transmit/receive interrupts 
   // NOTE: need to enable both RXF and RXB, but only TXB, as RXB does not get generated with RXF, 
   //       but TXB does get generated with TXF
   // However, on 52259, enabling RXB is resulting in an HBERR interrupt. RXB is not required, so leave it disabled.
   macnet_ptr->EIMR = ENET_EIR_TXB_MASK | ENET_EIR_RXF_MASK; 

   // Enable MACNET 
   macnet_ptr->ECR = (ENET_ECR_ETHEREN_MASK | ENET_ECR_EN1588_MASK);// | 0x100;

   // Discover PHY address if PHY_DISCOVER option is set
   if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PHY_DISCOVER) {
      bOK = (*enet_ptr->PARAM_PTR->ENET_IF->PHY_IF->DISCOVER)(enet_ptr);
      IF_ERROR_EXIT(!bOK, ENETERR_INIT_FAILED);
   } else {
      // Set Phy address from initialization parameter
      enet_ptr->PHY_ADDRESS = enet_ptr->PARAM_PTR->ENET_IF->PHY_ADDRESS;
   }
   
   // Enable STORE & FORWARD mode
   if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_STORE_AND_FORW) {
      macnet_ptr->TFWR |= ENET_TFWR_STRFWD_MASK;
      macnet_ptr->RSFL |= ENET_RSFL_RX_SECTION_FULL(0);
   }
   
   // Perform Phy initialization
   bOK = (*enet_ptr->PARAM_PTR->ENET_IF->PHY_IF->INIT)(enet_ptr);
   IF_ERROR_EXIT(!bOK, ENETERR_INIT_FAILED);


   // Signals the MACNET that empty buffers are available.
   // It is NECESSARY to do this AFTER enabling the MACNET.
   macnet_ptr->RDAR = ENET_RDAR_RDAR_MASK;
   
   //jm _mem_copy((void*)0x400d0000, (void*)0x3f400000, 0x400);


#if ENETCFG_SUPPORT_PTP
   macnet_ptp_ptr = _mem_alloc_system_zero(sizeof(MACNET_PTP_PRIVATE));
   IF_ERROR_EXIT((NULL==macnet_context_ptr), ENETERR_ALLOC_MAC_CONTEXT);


   macnet_ptp_ptr->MACNET_PTR = macnet_context_ptr->MACNET_ADDRESS;
   macnet_ptp_ptr->PTIMER_PRESENT = 1;

   bOK = _lwevent_create(&(macnet_ptp_ptr->LWEVENT_PTP), 0);
   IF_ERROR_EXIT(bOK, ENETERR_1588_LWEVENT);
   
   macnet_ptp_ptr->TXSTAMP = (MACNET_PTP_TIME){0,0};
   macnet_ptp_ptr->L2PCKS_PTR = NULL;
   
   macnet_context_ptr->PTP_PRIV = macnet_ptp_ptr;
   MACNET_ptp_init(enet_ptr);
   
   if (macnet_ptp_ptr->PTIMER_PRESENT) {
      /* Set Timer count */
      MACNET_ptp_start(macnet_ptp_ptr, (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PTP_MASTER_CLK));
      if(enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PTP_MASTER_CLK)
          MACNET_ptp_set_master_base_address(macnet_ptr);
      macnet_ptp_ptr->MACNET_PTR->ECR |= ENET_ECR_EN1588_MASK;

   }
   /* Enable timer-relative interrupts */
   macnet_ptp_ptr->MACNET_PTR->EIMR |= (ENET_EIR_TS_TIMER_MASK | ENET_EIR_TS_AVAIL_MASK);
#endif /* ENETCFG_SUPPORT_PTP */


   // control transfers to this point on any error, with error set to error code.
EXIT:
   if (ENET_OK!=error) {
      #if BSPCFG_ENET_RESTORE
         MACNET_uninstall_all_isrs(enet_ptr);
      #endif
      MACNET_free_context(macnet_context_ptr);
   }
   MACNET_int_enable();
   return error;
} 

/* EOF */
