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
* $FileName: macnet_mii.c$
* $Version : 3.8.8.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the MACNET MII Interface functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "macnet_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_mii_enabled
*  Returned Value : boolean
*  Comments       :
*    We can only read the PHY registers if the PHY is enabled
*
*END*-----------------------------------------------------------------*/

static boolean MACNET_mii_enabled
   (
		   ENET_MemMapPtr macnet_ptr
   )
{  
   return (macnet_ptr->MSCR & 0x7E) != 0;
} 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_read_write_mii
*  Returned Value : TRUE and MII Data register value, FALSE if timeout
*  Comments       :
*    Return entire MII data register value
*
*END*-----------------------------------------------------------------*/

static boolean MACNET_read_write_mii
   (
      ENET_MemMapPtr             macnet_ptr,
      uchar                      phy_addr,
      uint_32                    reg_index,
      uint_32                    op,
      uint_32                    write_data,
      uint_32_ptr                read_data_ptr,
      uint_32                    timeout
   )
{
   uint_32                    tm;
    
   if (macnet_ptr == NULL) 
      return FALSE;
   
   if (!MACNET_mii_enabled(macnet_ptr))
        return FALSE;
    
   // Clear the MII interrupt bit 
   macnet_ptr->EIR = ENET_EIR_MII_MASK;

   // Kick-off the MII read or write operation 
   macnet_ptr->MMFR = (vuint_32)(0 
      | (ENET_MMFR_ST_MASK & (0x01 << ENET_MMFR_ST_SHIFT))
      | op
      | (ENET_MMFR_PA_MASK & (phy_addr << ENET_MMFR_PA_SHIFT))
      | (ENET_MMFR_RA_MASK & (reg_index << ENET_MMFR_RA_SHIFT))
      | (ENET_MMFR_TA_MASK & (0x02 << ENET_MMFR_TA_SHIFT))
      | (write_data & 0xffff));

    // Poll for MII complete
   for (tm = 0; tm < timeout; tm++)
   {
      if(macnet_ptr->EIR & ENET_EIR_MII_MASK)
         break;
      _time_delay(0);
   }

   if (tm != timeout) 
      if (read_data_ptr) 
         *read_data_ptr = (ENET_MMFR_DATA_MASK & (macnet_ptr->MMFR));

   // Clear the MII interrupt bit 
   macnet_ptr->EIR = ENET_EIR_MII_MASK;

   return (tm != timeout);
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_read_mii
*  Returned Value : TRUE and MII Data register value, FALSE if timeout
*  Comments       :
*    Return entire MII data register value
*
*END*-----------------------------------------------------------------*/

boolean MACNET_read_mii
    (
        ENET_CONTEXT_STRUCT_PTR enet_ptr,
        uint_32 reg_index,
        uint_32_ptr data,
        uint_32 timeout
    )
{
    MACNET_CONTEXT_STRUCT_PTR macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;

    return MACNET_read_write_mii(macnet_context_ptr->PHY_PTR, enet_ptr->PHY_ADDRESS, reg_index, ENET_MMFR_OP_MASK & (0x02 << ENET_MMFR_OP_SHIFT), 0, data, timeout);
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_write_mii
*  Returned Value : TRUE if success, FALSE if timeout
*  Comments       :
*    Write MII data register value
*
*END*-----------------------------------------------------------------*/

boolean MACNET_write_mii
    (
        ENET_CONTEXT_STRUCT_PTR enet_ptr,
        uint_32 reg_index,
        uint_32 data,
        uint_32 timeout
    )
{
    MACNET_CONTEXT_STRUCT_PTR macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;

    return MACNET_read_write_mii(macnet_context_ptr->PHY_PTR, enet_ptr->PHY_ADDRESS, reg_index, ENET_MMFR_OP_MASK & (0x01 << ENET_MMFR_OP_SHIFT), data, NULL, timeout);
}
