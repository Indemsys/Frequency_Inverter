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
* $FileName: macnet_join.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the Ethernet multicasting
*   support functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "enet.h"
#include "enetprv.h"
#include "macnet_prv.h"          

#if BSPCFG_ENABLE_ENET_MULTICAST

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_join
*  Returned Value : void
*  Comments       :
*        Joins a multicast group on an Ethernet channel.
*
*END*-----------------------------------------------------------------*/

uint_32 MACNET_join
   (
     ENET_CONTEXT_STRUCT_PTR  enet_ptr,
         /* [IN] the Ethernet state structure */
      ENET_MCB_STRUCT_PTR     mcb_ptr
         /* [IN] the multicast control block */
   )
{ 
   MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   ENET_MemMapPtr             macnet_ptr         = macnet_context_ptr->MACNET_ADDRESS;
   uint_32                    i,j,crc            = 0xFFFFFFFFL;
   
   if (macnet_ptr == NULL) return ENETERR_INVALID_DEVICE;

   /* Compute the CRC-32 polynomial on the multicast group */
   for (i=0; i<6; i++) {
      uchar c = mcb_ptr->GROUP[i];
      for (j=0; j<8; j++) {
         if ((c ^ crc) & 1) {
            crc >>= 1;
            c >>= 1;
            crc ^= 0xEDB88320L;
         } else {
            crc >>= 1;
            c >>= 1;
         }  
      } 
   }  

   /* Set the appropriate bit in the hash table */
   crc >>= 26;
   crc &= 0x3F;
   mcb_ptr->HASH = crc;

   if (crc < 32) {
      macnet_ptr->GALR |= 0x1 << (crc & 0x1F);
   } else {
      macnet_ptr->GAUR |= 0x1 << (crc & 0x1F);
   }  

   return ENET_OK;
} 

#endif

/* EOF */
