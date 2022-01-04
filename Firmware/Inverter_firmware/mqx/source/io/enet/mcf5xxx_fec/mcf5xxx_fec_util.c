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
* $FileName: mcf5xxx_fec_util.c$
* $Version : 3.6.5.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the MCF5xxx Coldfire processor
*   Ethernet initialization
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enetprv.h"
#include "mcf5xxx_fec_prv.h"          


/*NOTIFIER*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_Error_ISR
*  Returned Value : void
*  Comments       :
*        Interrupt service routine for Tx.
*
*END*-----------------------------------------------------------------*/

static void MCF5XXX_FEC_Error_ISR 
   (
         /* [IN] the Ethernet state structure */
      pointer  enet
   )
{    
   ENET_CONTEXT_STRUCT_PTR          enet_ptr = (ENET_CONTEXT_STRUCT_PTR)enet;
   MCF5XXX_FEC_CONTEXT_STRUCT_PTR   fec_context_ptr = (MCF5XXX_FEC_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   VMCF5XXX_FEC_STRUCT_PTR          fec_ptr= fec_context_ptr->FEC_ADDRESS;
   uint_32                          events;
   
   if (fec_ptr == NULL) return;
   
   events = fec_ptr->EIR;
   while (events & MCF5XXX_FEC_EIR_HBERR) {

      /* clear the interrupt */
      fec_ptr->EIR = MCF5XXX_FEC_EIR_HBERR;

      events = fec_ptr->EIR;
   } 
} 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_install_isr
*  Returned Value : ENET_OK or error code
*  Comments       :
*         
*
*END*-----------------------------------------------------------------*/

static boolean MCF5XXX_FEC_install_isr( 
   ENET_CONTEXT_STRUCT_PTR enet_ptr, 
   uint_32                 int_num, 
   uint_32                 int_index, 
   INT_ISR_FPTR            isr, 
   uint_32                 level, 
   uint_32                 sublevel  ) 
{
   uint_32  vector = MCF5XXX_FEC_get_vector(enet_ptr->PARAM_PTR->ENET_IF->MAC_NUMBER, int_index);
 
#if BSPCFG_ENET_RESTORE
   MCF5XXX_FEC_CONTEXT_STRUCT_PTR    fec_context_ptr = (MCF5XXX_FEC_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;


   // Save old ISR and data
   fec_context_ptr->OLDISR_PTR[int_num]   = _int_get_isr(vector);
   fec_context_ptr->OLDISR_DATA[int_num] = _int_get_isr_data(vector);
#endif

   if (_int_install_isr(vector, isr, (pointer)enet_ptr)==NULL) {
      return FALSE;
   }

   // Initialize interrupt priority and level
   _psp_set_int_prio_and_level((PSP_INTERRUPT_TABLE_INDEX)vector, level, sublevel, TRUE);

   return TRUE;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_install_isrs
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/

boolean MCF5XXX_FEC_install_isrs( ENET_CONTEXT_STRUCT_PTR enet_ptr, MCF5XXX_FEC_INIT_STRUCT const * enet_init_ptr  ) 
{
   boolean bOK;
   bOK = MCF5XXX_FEC_install_isr(enet_ptr, 0, MCF5XXX_FEC_INT_TX_INTB, MCF5XXX_FEC_TX_ISR, enet_init_ptr->ETX_LEVEL,enet_init_ptr->ETX_SUBLEVEL  ) ;
   if (bOK) {
      bOK = MCF5XXX_FEC_install_isr(enet_ptr, 1, MCF5XXX_FEC_INT_TX_INTF, MCF5XXX_FEC_TX_ISR, enet_init_ptr->ETX_LEVEL,enet_init_ptr->ETX_SUBLEVEL  ) ;
   }
   if (bOK) {
      bOK = MCF5XXX_FEC_install_isr(enet_ptr, 2, MCF5XXX_FEC_INT_RX_INTB, MCF5XXX_FEC_RX_ISR, enet_init_ptr->ERX_LEVEL,enet_init_ptr->ERX_SUBLEVEL  ) ;
   }
   if (bOK) {
      bOK = MCF5XXX_FEC_install_isr(enet_ptr, 3, MCF5XXX_FEC_INT_RX_INTF, MCF5XXX_FEC_RX_ISR, enet_init_ptr->ERX_LEVEL,enet_init_ptr->ERX_SUBLEVEL  ) ;
   }
   
#if !PSP_MQX_CPU_IS_MCF5441X
   if (bOK) {
      bOK = MCF5XXX_FEC_install_isr(enet_ptr, 4, MCF5XXX_FEC_INT_HBERR, MCF5XXX_FEC_Error_ISR, enet_init_ptr->ERX_LEVEL,enet_init_ptr->ERX_SUBLEVEL  ) ;
   }
#endif 

   return bOK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_mask_interrupts
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/

void MCF5XXX_FEC_mask_interrupts( ENET_CONTEXT_STRUCT_PTR enet_ptr ) 
{
   uint_32  i;
   
   for (i=0;i<MCF5XXX_FEC_NUM_INTS;i++) {
      _psp_int_mask((PSP_INTERRUPT_TABLE_INDEX)(MCF5XXX_FEC_get_vector(enet_ptr->PARAM_PTR->ENET_IF->MAC_NUMBER,i)) );
   }
}


#if BSPCFG_ENET_RESTORE

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_uninstall_isr
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/

static void MCF5XXX_FEC_uninstall_isr( ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 int_num, uint_32 int_index  ) 
{
   uint_32  vector = MCF5XXX_FEC_get_vector(enet_ptr->PARAM_PTR->ENET_IF->MAC_NUMBER, int_index);
   MCF5XXX_FEC_CONTEXT_STRUCT_PTR    fec_context_ptr = (MCF5XXX_FEC_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
 
   _psp_int_mask((PSP_INTERRUPT_TABLE_INDEX)vector);
   if (fec_context_ptr->OLDISR_PTR[int_num]) {
      _int_install_isr(vector, fec_context_ptr->OLDISR_PTR[int_num], fec_context_ptr->OLDISR_DATA[int_num]);
      fec_context_ptr->OLDISR_PTR[int_num] = (INT_ISR_FPTR)NULL;
   }
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_uninstall_all_isrs
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/

void MCF5XXX_FEC_uninstall_all_isrs( ENET_CONTEXT_STRUCT_PTR enet_ptr ) 
{
   MCF5XXX_FEC_uninstall_isr(enet_ptr, 0, MCF5XXX_FEC_INT_TX_INTB) ;
   MCF5XXX_FEC_uninstall_isr(enet_ptr, 1, MCF5XXX_FEC_INT_TX_INTF) ;
   MCF5XXX_FEC_uninstall_isr(enet_ptr, 2, MCF5XXX_FEC_INT_RX_INTB) ;
   MCF5XXX_FEC_uninstall_isr(enet_ptr, 3, MCF5XXX_FEC_INT_RX_INTF) ;
   MCF5XXX_FEC_uninstall_isr(enet_ptr, 4, MCF5XXX_FEC_INT_RX_INTF) ;
}
#endif


/*FUNCTION*-------------------------------------------------------------
* 
* Function Name    : MCF5XXX_FEC_get_vector
* Returned Value   : MQX vector number for specified interrupt
* Comments         :
*    This function returns index into MQX interrupt vector table for
*    specified enet interrupt. If not known, returns 0.
*
*END*-----------------------------------------------------------------*/


uint_32 MCF5XXX_FEC_get_vector 
(
    uint_32 device,
    uint_32 vector_index
)
{ 
    uint_32 index = 0;

    if ((device < MCF5XXX_FEC_DEVICE_COUNT) && (vector_index < MCF5XXX_FEC_NUM_INTS)) {
      index = MCF5XXX_FEC_vectors[device][vector_index];
    }

    return index;
} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_free_context
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/

void MCF5XXX_FEC_free_context( MCF5XXX_FEC_CONTEXT_STRUCT_PTR fec_context_ptr ) 
{
   if (fec_context_ptr) {
      if (fec_context_ptr->UNALIGNED_BUFFERS) {
         _mem_free((pointer)fec_context_ptr->UNALIGNED_BUFFERS);
      }
      if (fec_context_ptr->RX_PCB_BASE) {
         _mem_free((pointer)fec_context_ptr->RX_PCB_BASE);
      }
      if (fec_context_ptr->TxPCBS_PTR) {
         _mem_free((pointer)fec_context_ptr->TxPCBS_PTR);
      }
      if (fec_context_ptr->UNALIGNED_RING_PTR) {
         _mem_free((pointer)fec_context_ptr->UNALIGNED_RING_PTR);
      }
   
      _mem_free((pointer)fec_context_ptr);
   }
}


/* EOF */
