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
* $FileName: eninit.c$
* $Version : 3.8.12.0$
* $Date    : Sep-11-2012$
*
* Comments:
*
*   This file contains the MCF52xx Coldfire processor
*   Ethernet initialization
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "bsp_prv.h"




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_initialize_ex
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Initializes the chip (extended version).
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_initialize_ex
   (
         /* [IN] optional parameters */
      const ENET_PARAM_STRUCT *   param_ptr,
         /* [IN] the local Ethernet address */
      _enet_address           address,
         /* [OUT] the Ethernet state structure */
      _enet_handle _PTR_      handle
   )
{
   ENET_CONTEXT_STRUCT_PTR enet_ptr = NULL;//, other_enet_ptr;
   uint_32                 result;
   boolean                 vlan;
   
   if (param_ptr == NULL)
      return ENETERR_INVALID_DEVICE;

   if (param_ptr->NUM_RX_BUFFERS < param_ptr->NUM_RX_ENTRIES) 
      return ENETERR_INVALID_INIT_PARAM;
   
   enet_ptr = _mqx_get_io_component_handle(IO_ENET_COMPONENT);
   
   while (enet_ptr) {
      if (enet_ptr->PARAM_PTR->ENET_IF->MAC_NUMBER == param_ptr->ENET_IF->MAC_NUMBER)
         break;
      
      enet_ptr = enet_ptr->NEXT;
   }
   
   if (enet_ptr) {
      *handle = enet_ptr;
   
      return ENETERR_INITIALIZED_DEVICE;
   }
   else
      *handle = NULL;

   // Allocate the Enet context structure
   enet_ptr = _mem_alloc_system_zero(sizeof(ENET_CONTEXT_STRUCT));
   if (NULL==enet_ptr)
      return ENETERR_ALLOC_CFG;
      
   _mem_set_type((pointer)enet_ptr, MEM_TYPE_IO_ENET_CONTEXT_STRUCT);

   // Initialize the Enet context structure
   eaddrcpy(enet_ptr->ADDRESS, address);
   enet_ptr->PARAM_PTR = param_ptr;
   vlan = (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_VLAN) == ENET_OPTION_VLAN;
   enet_ptr->MaxTxFrameSize = ENET_max_framesize(enet_ptr->PARAM_PTR->TX_BUFFER_SIZE,0,vlan);
   enet_ptr->MaxRxFrameSize = ENET_max_framesize(enet_ptr->PARAM_PTR->RX_BUFFER_SIZE,enet_ptr->PARAM_PTR->NUM_LARGE_BUFFERS,vlan);
   
   _lwsem_create(&enet_ptr->CONTEXT_LOCK, 1);
    
   // Initialize the MAC
   result = (*param_ptr->ENET_IF->MAC_IF->INIT)(enet_ptr);
   
    if (ENET_OK == result)
    {
        // Link the driver into the kernel component list
        _mqx_link_io_component_handle(IO_ENET_COMPONENT,enet_ptr, (pointer *)&enet_ptr->NEXT);

        *handle = enet_ptr;
    } 
    else
    {
        _lwsem_destroy(&enet_ptr->CONTEXT_LOCK);
        _mem_free(enet_ptr);
        *handle = NULL;
    }
   
    return result;
}   

 
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_initialize
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Initializes the chip.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_initialize
   (
         /* [IN] the FEC to initialize */
      uint_32              device,
         /* [IN] the local Ethernet address */
      _enet_address        address,
         /* [IN] optional flags, zero = default, 
            this parameter IS NOT USED! (ignored) */
      uint_32              flags,
         /* [OUT] the Ethernet state structure */
      _enet_handle _PTR_   handle
   )
{ 
   if (device <BSP_ENET_DEVICE_COUNT) {
      return ENET_initialize_ex(&ENET_default_params[device],address,handle);
   } else {
      return ENETERR_INVALID_DEVICE;
   }
}

/* EOF */
