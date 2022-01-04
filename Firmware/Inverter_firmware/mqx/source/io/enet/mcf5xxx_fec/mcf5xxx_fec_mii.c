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
* $FileName: mcf5xxx_fec_mii.c$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the MCF5xxx Ethernet MII Interface functions
*   functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "mcf5xxx_fec_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_mii_enabled
*  Returned Value : boolean
*  Comments       :
*    We can only read the PHY registers if the PHY is enabled
*
*END*-----------------------------------------------------------------*/

static boolean MCF5XXX_FEC_mii_enabled
   (
      VMCF5XXX_FEC_STRUCT_PTR fec_ptr
   )
{  
   return (fec_ptr->MSCR & 0x7E) != 0;
} 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_read_write_mii
*  Returned Value : TRUE and MII Data register value, FALSE if timeout
*  Comments       :
*    Return entire MII data register value
*
*END*-----------------------------------------------------------------*/

static boolean MCF5XXX_FEC_read_write_mii
   (
      VMCF5XXX_FEC_STRUCT_PTR    fec_ptr,
      uchar                      phy_addr,
      uint_32                    reg_index,
      uint_32                    op,
      uint_32                    write_data,
      uint_32_ptr                read_data_ptr,
      uint_32                    timeout
   )
{
   uint_32                    tm;
    
   if (fec_ptr == NULL) 
      return FALSE;
   
   if (!MCF5XXX_FEC_mii_enabled(fec_ptr))
        return FALSE;
    
   // Clear the MII interrupt bit 
   fec_ptr->EIR = MCF5XXX_FEC_EIR_MII;

   // Kick-off the MII read or write operation 
   fec_ptr->MMFR = (vuint_32)(0 
      | MCF5XXX_FEC_MII_START_OF_FRAME
      | op
      | MCF5XXX_FEC_MII_PA(phy_addr)
      | MCF5XXX_FEC_MII_RA(reg_index)
      | MCF5XXX_FEC_MII_TURNAROUND
      | (write_data & 0xffff));

    // Poll for MII complete
   for (tm = 0; tm < timeout; tm++)
   {
      if(fec_ptr->EIR & MCF5XXX_FEC_EIR_MII)
         break;
      _time_delay(0);
   }

   if (tm != timeout) 
      if (read_data_ptr) 
         *read_data_ptr = MCF5XXX_FEC_MII_DATA(fec_ptr->MMFR);

   // Clear the MII interrupt bit 
   fec_ptr->EIR = MCF5XXX_FEC_EIR_MII;

   return (tm != timeout);
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_read_mii
*  Returned Value : TRUE and MII Data register value, FALSE if timeout
*  Comments       :
*    Return entire MII data register value
*
*END*-----------------------------------------------------------------*/

boolean MCF5XXX_FEC_read_mii
    (
        ENET_CONTEXT_STRUCT_PTR enet_ptr,
        uint_32 reg_index,
        uint_32_ptr data,
        uint_32 timeout
    )
{
    MCF5XXX_FEC_CONTEXT_STRUCT_PTR fec_context_ptr = (MCF5XXX_FEC_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;

    return MCF5XXX_FEC_read_write_mii(fec_context_ptr->PHY_PTR, enet_ptr->PHY_ADDRESS, reg_index, MCF5XXX_FEC_MII_OPCODE_READ, 0, data, timeout);
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MCF5XXX_FEC_write_mii
*  Returned Value : TRUE if success, FALSE if timeout
*  Comments       :
*    Write MII data register value
*
*END*-----------------------------------------------------------------*/

boolean MCF5XXX_FEC_write_mii
    (
        ENET_CONTEXT_STRUCT_PTR enet_ptr,
        uint_32 reg_index,
        uint_32 data,
        uint_32 timeout
    )
{
    MCF5XXX_FEC_CONTEXT_STRUCT_PTR fec_context_ptr = (MCF5XXX_FEC_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;

    return MCF5XXX_FEC_read_write_mii(fec_context_ptr->PHY_PTR, enet_ptr->PHY_ADDRESS, reg_index, MCF5XXX_FEC_MII_OPCODE_WRITE, data, NULL, timeout);
}
