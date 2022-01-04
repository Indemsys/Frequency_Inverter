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
* $FileName: macnet_stop.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the MACNET shutdown functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "macnet_prv.h"          

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_shutdown
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Stops the chip.
*
*END*-----------------------------------------------------------------*/

uint_32 MACNET_shutdown
   (
      ENET_CONTEXT_STRUCT_PTR  enet_ptr
         /* [IN] the Ethernet state structure */
   )
{ /* Body */
   MACNET_CONTEXT_STRUCT_PTR   macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   ENET_MemMapPtr              macnet_ptr= macnet_context_ptr->MACNET_ADDRESS;

   if (macnet_ptr == NULL) return ENETERR_INVALID_DEVICE;

   /* Stop the chip */
   macnet_ptr->ECR = ENET_ECR_RESET_MASK;

   /* Disable all MACNET interrupts */
   macnet_ptr->EIMR = 0;

   /* clear any pending interrpts */
   macnet_ptr->EIR = ENET_EIR_ALL_PENDING;

   /* Stop the MACNET from interrupting the core */
    MACNET_mask_interrupts( enet_ptr );

   #if BSPCFG_ENET_RESTORE
      /* Restore old ISRs */
      MACNET_uninstall_all_isrs( enet_ptr );
   #endif

    /*
    ** Make sure all PCBs are free
    */
    if (macnet_context_ptr->ActiveRxBDs != macnet_context_ptr->NumRxBDs) {
       return ENETERR_FREE_PCB;
    } 

   MACNET_free_context(macnet_context_ptr);
   return ENET_OK;

} 


/* EOF */
