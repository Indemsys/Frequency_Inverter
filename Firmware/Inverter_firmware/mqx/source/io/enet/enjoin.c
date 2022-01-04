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
* $FileName: enjoin.c$
* $Version : 3.5.9.0$
* $Date    : Nov-24-2009$
*
* Comments:
*
*   This file contains the Ethernet multicasting
*   interface function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"

#if BSPCFG_ENABLE_ENET_MULTICAST

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_join
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Joins a multicast group on an Ethernet channel.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_join
   (
      /* [IN] the Ethernet state structure */
      _enet_handle      handle,

      /* [IN] the protocol */
      uint_16           type,

      /* [IN] the multicast group */
      _enet_address     address
   )
{ 
   ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;
   ENET_ECB_STRUCT_PTR     ecb_ptr;
   ENET_MCB_STRUCT_PTR     mcb_ptr;
   #ifdef ENET_ALLMCAST
      boolean  needjoin;
   #endif

   /*
   ** Make sure it's a multicast group
   */
   #if MQX_CHECK_ERRORS
      if ((address[0] & 1) == 0) {
         return ENETERR_JOIN_MULTICAST;
      } /* Endif */
   #endif

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself and with ENET_leave.
   */
   ENET_lock_context(enet_ptr);

   /*
   ** Make sure it's an open protocol
   */
   for (ecb_ptr = enet_ptr->ECB_HEAD;
        ecb_ptr;
        ecb_ptr = ecb_ptr->NEXT) {

      if (ecb_ptr->TYPE == type) {
         /* Found an existing entry */
         break;
      } 
   } 

   /*
   ** No existing entry found
   */
   if (!ecb_ptr) {
      ENET_unlock_context(enet_ptr);
      return ENETERR_CLOSE_PROT;
   } 

   /*
   ** Create an entry for this group
   */
   mcb_ptr = _mem_alloc_system_zero(sizeof(ENET_MCB_STRUCT));
   if (!mcb_ptr) {
      ENET_unlock_context(enet_ptr);
      return ENETERR_ALLOC_MCB;
   } /* Endif */
   _mem_set_type(mcb_ptr, MEM_TYPE_IO_ENET_MCB);
   
   eaddrcpy(mcb_ptr->GROUP,address);

   mcb_ptr->NEXT = ecb_ptr->MCB_HEAD;
   ecb_ptr->MCB_HEAD = mcb_ptr;

#if BSPCFG_ENABLE_ENET_MULTICAST
   #ifdef ENET_ALLMCAST
      needjoin = enet_ptr->MCOUNT < ENET_ALLMCAST;
      enet_ptr->MCOUNT++;
      if (needjoin) {
   #endif
   
         (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->JOIN)(handle, mcb_ptr);
   
   #ifdef ENET_ALLMCAST
      } 
   #endif
#endif

   ENET_unlock_context(enet_ptr);

   return ENET_OK;

} 

#endif

/* EOF */
