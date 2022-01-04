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
* $FileName: mcf5xxx_fec_stop.c$
* $Version : 3.0.1.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   This file contains the MCF5xxx Ethernet shutdown
*   functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "mcf5xxx_fec_prv.h"          

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_shutdown
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Stops the chip.
*
*END*-----------------------------------------------------------------*/

uint_32 MCF5XXX_FEC_shutdown
   (
      ENET_CONTEXT_STRUCT_PTR  enet_ptr
         /* [IN] the Ethernet state structure */
   )
{ /* Body */
   MCF5XXX_FEC_CONTEXT_STRUCT_PTR    fec_context_ptr = (MCF5XXX_FEC_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   VMCF5XXX_FEC_STRUCT_PTR           fec_ptr= fec_context_ptr->FEC_ADDRESS;

   if (fec_ptr == NULL) return ENETERR_INVALID_DEVICE;

   /* Stop the chip */
   fec_ptr->ECR = MCF5XXX_FEC_ECR_RESET;

   /* Disable all FEC interrupts */
   fec_ptr->EIMR = 0;

   /* clear any pending interrpts */
   fec_ptr->EIR = MCF5XXX_FEC_EIR_ALL_PENDING;

   /* Stop the FEC from interrupting the core */
    MCF5XXX_FEC_mask_interrupts( enet_ptr );

   #if BSPCFG_ENET_RESTORE
      /* Restore old ISRs */
      MCF5XXX_FEC_uninstall_all_isrs( enet_ptr );
   #endif

    /*
    ** Make sure all PCBs are free
    */
    if (fec_context_ptr->ActiveRxBDs != fec_context_ptr->NumRxBDs) {
       return ENETERR_FREE_PCB;
    } 

   MCF5XXX_FEC_free_context(fec_context_ptr);
   return ENET_OK;

} 


/* EOF */
