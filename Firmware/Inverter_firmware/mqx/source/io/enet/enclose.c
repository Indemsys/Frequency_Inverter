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
* $FileName: enclose.c$
* $Version : 3.5.7.0$
* $Date    : Nov-24-2009$
*
* Comments:
*
*   This file contains the Ethernet close
*   interface function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_close
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Unregisters a protocol type on an Ethernet channel.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_close
   (
      /* [IN] the Ethernet state structure */
      _enet_handle      handle,

      /* [IN] the protocol */
      uint_16           type
   )
{ 
   ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;
   ENET_ECB_STRUCT_PTR     ecb_ptr, _PTR_ search_ptr;
   ENET_MCB_STRUCT_PTR     mcb_ptr, next_ptr;

   #if BSPCFG_ENABLE_ENET_MULTICAST
      #ifdef ENET_ALLMCAST
            uint_32        mcount;
            boolean        needrejoin;
      #endif
   #endif

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself, ENET_open(), ENET_join(), ENET_leave(),
   ** and ENET_ISR().
   */
   ENET_lock_context(enet_ptr);

   // Search for an existing entry for type
   for (search_ptr = (ENET_ECB_STRUCT_PTR _PTR_)&enet_ptr->ECB_HEAD;
       *search_ptr; search_ptr = &(*search_ptr)->NEXT) 
   {

      if ((*search_ptr)->TYPE == type) {
         // Found an existing entry -- delete it
         break;
      }  
   } 

   if (!*search_ptr) {
      // No existing entry found
      ENET_unlock_context(enet_ptr);
      return ENETERR_CLOSE_PROT;
   } 

   ecb_ptr = *search_ptr;
   *search_ptr = ecb_ptr->NEXT;

   #if BSPCFG_ENABLE_ENET_MULTICAST
      #ifdef ENET_ALLMCAST
         mcb_ptr = ecb_ptr->MCB_HEAD;
         mcount = 0;
         while (mcb_ptr) {
            mcount++;
            mcb_ptr = mcb_ptr->NEXT;
         } 
         enet_ptr->MCOUNT -= mcount;
         needrejoin = mcount && (enet_ptr->MCOUNT < ENET_ALLMCAST);
      #endif
   #endif

   ENET_unlock_context(enet_ptr);

   mcb_ptr = ecb_ptr->MCB_HEAD;
   _mem_free(ecb_ptr);
   while (mcb_ptr) {
      next_ptr = mcb_ptr->NEXT;
      _mem_free(mcb_ptr);
      mcb_ptr = next_ptr;
   } 

   #if BSPCFG_ENABLE_ENET_MULTICAST
      #ifdef ENET_ALLMCAST
         if (needrejoin) {
      #endif
            ENET_lock_context(enet_ptr);
            (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->REJOIN)(handle);
            ENET_unlock_context(enet_ptr);
      #ifdef ENET_ALLMCAST
         }
      #endif
   #endif

   return ENET_OK;

}


/* EOF */
