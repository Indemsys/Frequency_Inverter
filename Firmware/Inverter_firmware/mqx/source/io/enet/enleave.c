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
* $FileName: enleave.c$
* $Version : 3.5.7.0$
* $Date    : Nov-24-2009$
*
* Comments:
*
*   This file contains the Ethernet multicasting
*   interface functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"

#if BSPCFG_ENABLE_ENET_MULTICAST

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_leave
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Leaves a multicast group on an Ethernet channel.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_leave
   (
      /* [IN] the Ethernet state structure */
      _enet_handle      handle,

      /* [IN] the protocol */
      uint_16           type,

      /* [IN] the multicast group */
      _enet_address     address
   )
{ 
   ENET_CONTEXT_STRUCT_PTR  enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;
   ENET_ECB_STRUCT_PTR  ecb_ptr;
   ENET_MCB_STRUCT_PTR  mcb_ptr, _PTR_ search_ptr;
   #ifdef ENET_ALLMCAST
      boolean  needrejoin;
   #endif

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself and with ENET_Join.
   */
   ENET_lock_context(enet_ptr);

   // Make sure it's an open protocol
   for (ecb_ptr = enet_ptr->ECB_HEAD;ecb_ptr;ecb_ptr = ecb_ptr->NEXT) {
      if (ecb_ptr->TYPE == type) {
         /* Found an existing entry */
         break;
      } 
   } 

   // No existing entry found
   if (!ecb_ptr) {
      ENET_unlock_context(enet_ptr);
      return ENETERR_CLOSE_PROT;
   } 

   // Make sure it's a joined group
   for (search_ptr = &ecb_ptr->MCB_HEAD;
       *search_ptr;
        search_ptr = &(*search_ptr)->NEXT) {

      if (((*search_ptr)->GROUP[0] == address[0])
       && ((*search_ptr)->GROUP[1] == address[1])
       && ((*search_ptr)->GROUP[2] == address[2])
       && ((*search_ptr)->GROUP[3] == address[3])
       && ((*search_ptr)->GROUP[4] == address[4])
       && ((*search_ptr)->GROUP[5] == address[5])) {
         /* Found the entry -- delete it */
         break;
      } 
   } 

   if (!*search_ptr) {
      ENET_unlock_context(enet_ptr);
      return ENETERR_LEAVE_GROUP;
   } 

   mcb_ptr = *search_ptr;
   *search_ptr = mcb_ptr->NEXT;

   #ifdef ENET_ALLMCAST
      enet_ptr->MCOUNT--;
      needrejoin = enet_ptr->MCOUNT < ENET_ALLMCAST;

      if (needrejoin) {
   #endif
         (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->REJOIN)(handle);
   #ifdef ENET_ALLMCAST
      } 
   #endif

   ENET_unlock_context(enet_ptr);

   _mem_free(mcb_ptr);

   return ENET_OK;
} 

#endif

/* EOF */
