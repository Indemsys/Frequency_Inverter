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
* $FileName: mcf5xxx_fec_rejoin.c$
* $Version : 3.0.1.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   This file contains the MCF5xxx Ethernet multicasting
*   interface functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "mcf5xxx_fec_prv.h"          

#if BSPCFG_ENABLE_ENET_MULTICAST

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_rejoin
*  Returned Value : void
*  Comments       :
*        Rejoins all joined multicast groups.  Called by ENET_close
*        and ENET_leave.
*
*END*-----------------------------------------------------------------*/

uint_32 MCF5XXX_FEC_rejoin
   (
      ENET_CONTEXT_STRUCT_PTR  enet_ptr
         /* [IN] the Ethernet state structure */
   )
{ 
   MCF5XXX_FEC_CONTEXT_STRUCT_PTR   fec_context_ptr = (MCF5XXX_FEC_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   VMCF5XXX_FEC_STRUCT_PTR          fec_ptr= fec_context_ptr->FEC_ADDRESS;
   ENET_ECB_STRUCT_PTR            ecb_ptr;
   ENET_MCB_STRUCT_PTR            mcb_ptr;
   uint_32                        ialr = 0, iaur = 0;

   if (fec_ptr == NULL)  return ENETERR_INVALID_DEVICE;

   /*
   ** Add the remaining multicast groups to the group address filter
   */
   for (ecb_ptr = enet_ptr->ECB_HEAD;
        ecb_ptr;
        ecb_ptr = ecb_ptr->NEXT) {

      for (mcb_ptr = ecb_ptr->MCB_HEAD;
           mcb_ptr;
           mcb_ptr = mcb_ptr->NEXT) {

         uint_32 crc = mcb_ptr->HASH;

         if (crc < 32)  {
            ialr |= 1 << (crc & 0x1F);
         } else {
            iaur |= 1 << (crc & 0x1F);
         } 
      } 
   } 

   fec_ptr->GALR = ialr;
   fec_ptr->GAUR = iaur;

   return ENET_OK;
} 

#endif

/* EOF */
