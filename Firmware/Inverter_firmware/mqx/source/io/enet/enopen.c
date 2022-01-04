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
* $FileName: enopen.c$
* $Version : 3.8.11.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   This file contains the Ethernet open interface function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_open
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Registers a protocol type on an Ethernet channel.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_open
   (
      /* [IN] the Ethernet state structure */
      _enet_handle      handle,

      /* [IN] the protocol */
      uint_16           type,

      /* [IN] the callback function */
      void (_CODE_PTR_  service)(PCB_PTR, pointer),

      /* [IN] private data for the callback */
      pointer           prv
   )
{ 
   ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;
   ENET_ECB_STRUCT_PTR     ecb_ptr, _PTR_ search_ptr;

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself, ENET_close(), and ENET_ISR().
   */
   ENET_lock_context(enet_ptr);

   // Search for an existing entry for type
   for (search_ptr = (ENET_ECB_STRUCT_PTR _PTR_)&enet_ptr->ECB_HEAD; *search_ptr; search_ptr = &(*search_ptr)->NEXT) {
      if ((*search_ptr)->TYPE == type) {
         /* Found an existing entry */
         ENET_unlock_context(enet_ptr);
         return ENETERR_OPEN_PROT;
      } 
   } 

   // No existing entry found -- create a new one
   ecb_ptr = (ENET_ECB_STRUCT_PTR)_mem_alloc_system_zero(sizeof(ENET_ECB_STRUCT));
   if (!ecb_ptr) {
      ENET_unlock_context(enet_ptr);
      return ENETERR_ALLOC_ECB;
   } 
   _mem_set_type(ecb_ptr, MEM_TYPE_IO_ENET_ECB);
   
   ecb_ptr->TYPE     = type;
   ecb_ptr->SERVICE  = service;
   ecb_ptr->MCB_HEAD = NULL;
   ecb_ptr->PRIVATE  = prv;
   ecb_ptr->NEXT     = NULL;
   *search_ptr       = ecb_ptr;

   ENET_unlock_context(enet_ptr);
   return ENET_OK;

}


/* EOF */
