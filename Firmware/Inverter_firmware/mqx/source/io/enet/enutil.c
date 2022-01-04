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
* $FileName: enutil.c$
* $Version : 3.8.11.0$
* $Date    : Oct-2-2012$
*
* Comments:
*
*   This file contains misc. ENET driver utility functions
*   function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"


#if 0
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_get_device_number
*  Returned Value : device number or -1
*  Comments       :
*        Retrieves the Ethernet device number of an initialized device.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_get_device_number
   (
      /* [IN] the Ethernet state structure */
      _enet_handle   handle
   )
{
    ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;

    if (enet_ptr != NULL)
    {
        return enet_ptr->DEVICE;
    }
    return (uint_32)-1;
}
#endif

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_Enqueue_Buffer
*  Returned Value :
*  Comments       :
*
*
*END*-----------------------------------------------------------------*/

void ENET_Enqueue_Buffer( pointer * q, pointer b)
{
   *((pointer *)b) = *q;
   *q = b;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_Dequeue_Buffer
*  Returned Value :
*  Comments       :
*
*
*END*-----------------------------------------------------------------*/

pointer ENET_Dequeue_Buffer( pointer * q)
{
   pointer b = *q;

   if (b) {
      *q = *((pointer *)b);
   }
   return b;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_link_status
*  Returned Value : TRUE if link active, FALSE otherwise
*  Comments       : Get actual link status.
*
*END*-----------------------------------------------------------------*/

boolean ENET_link_status(_enet_handle   handle)
{
    ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;

   return (*enet_ptr->PARAM_PTR->ENET_IF->PHY_IF->STATUS)(enet_ptr);
}



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_get_speed
*  Returned Value : 10 or 100
*  Comments       : For Backward compatibility of old BSPs.
*        Get the negoiated speed.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_get_speed
   (
      /* [IN] the Ethernet state structure */
      _enet_handle   handle
   )
{
   ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;

   return (*enet_ptr->PARAM_PTR->ENET_IF->PHY_IF->SPEED)(enet_ptr);
}



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_max_framesize
*  Returned Value :
*  Comments       :
*
*     Calculate maximum frame size based on user parameters.
*  If the user specifies large buffers, then the max will be the largest
*  frame size (based on vlan).
*
*
*END*-----------------------------------------------------------------*/

uint_16 ENET_max_framesize(uint_16 best_buffer_size, uint_16 num_large_buffers, boolean vlan)
{

  if (num_large_buffers || (best_buffer_size==0)) {
      if (vlan) {
         return ENET_FRAMESIZE_VLAN;
      } else {
         return ENET_FRAMESIZE;
      }
   } else {
      return best_buffer_size;
   }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_get_MTU
*  Returned Value : mtu
*  Comments       :
*        Get the MTU.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_get_MTU
   (
      /* [IN] the Ethernet state structure */
      _enet_handle   handle
   )
{
   ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;
   uint_32  mtu = enet_ptr->PARAM_PTR->TX_BUFFER_SIZE;

   if (!mtu) {
      return ENET_FRAMESIZE_MAXDATA;  // The same regardless of whether VLAN is enabled.
   }

   if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_VLAN) {
      return(mtu - ENET_FRAMESIZE_HEAD_VLAN - ENET_FRAMESIZE_TAIL);
   } else {
      return(mtu - ENET_FRAMESIZE_HEAD - ENET_FRAMESIZE_TAIL);
   }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_read_mii
*  Returned Value : TRUE and MII Data register value, FALSE if timeout
*  Comments       :
*    Returns entire MII data register value.
*
*END*-----------------------------------------------------------------*/

boolean ENET_read_mii
    (
        /* [IN] the Ethernet state structure */
        _enet_handle handle,
        uint_32 reg_index,
        uint_32_ptr data,
        uint_32 timeout
    )
{
    ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;

    return (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, reg_index, data, timeout);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_write_mii
*  Returned Value : TRUE if succeeded, FALSE if timeout
*    Writes entire MII data register.
*
*END*-----------------------------------------------------------------*/

boolean ENET_write_mii
    (
        /* [IN] the Ethernet state structure */
        _enet_handle handle,
        uint_32 reg_index,
        uint_32 data,
        uint_32 timeout
    )
{
    ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;

    return (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_WRITE)(enet_ptr, reg_index, data, timeout);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_get_phy_addr
*  Returned Value : phy address or -1
*  Comments       :
*        Retrieves the PHY address of an initialized device.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_get_phy_addr
    (
        /* [IN] the Ethernet state structure */
        _enet_handle handle
    )
{
    ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;

    if (enet_ptr != NULL)
    {
        return (uint_32)(enet_ptr->PHY_ADDRESS);
    }
    return (uint_32)-1;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_phy_registers
*  Returned Value : phy registers
*  Comments       :
*        Retrieves the PHY registers.
*
*END*-----------------------------------------------------------------*/

boolean ENET_phy_registers
   (
      _enet_handle handle,
      uint_32      num_regs,
      uint_32_ptr  regs_ptr
   )
{
   ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;
   uint_32  i;
   boolean  all_read=TRUE;
   for (i=0;i<num_regs;i++) {
      *regs_ptr=0;
      if (!(*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, i, regs_ptr, 0x1000))
      {
         all_read=FALSE;
      }
      regs_ptr++;
   }
   return all_read;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_get_device_handle
*  Returned Value : device_handle
*  Comments       :
*        Retrieves the device_handle
*
*END*-----------------------------------------------------------------*/

_enet_handle ENET_get_device_handle(uint_32 mac_number)
{
   ENET_CONTEXT_STRUCT_PTR enet_ptr = NULL;
   uint_32                 result;
   boolean                 vlan;
   
   enet_ptr = _mqx_get_io_component_handle(IO_ENET_COMPONENT);
   
   while (enet_ptr) {
      if (enet_ptr->PARAM_PTR->ENET_IF->MAC_NUMBER == mac_number)
         return enet_ptr;
      
      enet_ptr = enet_ptr->NEXT;
   }
   
  return NULL;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_get_next_device_handle
*  Returned Value : device_handle
*  Comments       :
*        Retrieves the next device_handle
*
*END*-----------------------------------------------------------------*/
_enet_handle ENET_get_next_device_handle(_enet_handle handle)
{
   ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR) handle;
   
   if (enet_ptr != NULL) {
       return enet_ptr->NEXT;
   }
   return NULL;
}
