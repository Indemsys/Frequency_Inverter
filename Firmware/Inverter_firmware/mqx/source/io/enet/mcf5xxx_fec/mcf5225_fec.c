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
* $FileName: mcf5225_fec.c$
* $Version : 3.6.2.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   Processor family specific file needed for enet module.
*   Revision History:
*   Date             Version  Changes
*   ---------        -------  -------
*   Sep 26/08        2.50     Initial version
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <enet.h>
#include <enetprv.h>
#include <mcf5xxx_fec_prv.h>


const uint_32 MCF5XXX_FEC_vectors[MCF5XXX_FEC_DEVICE_COUNT][MCF5XXX_FEC_NUM_INTS] = {
    {
        MCF5225_INT_FEC_X_INTF,
        MCF5225_INT_FEC_X_INTB,
        MCF5225_INT_FEC_X_UN,
        MCF5225_INT_FEC_RL,
        MCF5225_INT_FEC_R_INTF,
        MCF5225_INT_FEC_R_INTB,
        MCF5225_INT_FEC_MII,
        MCF5225_INT_FEC_LC,
        MCF5225_INT_FEC_HBERR,
        MCF5225_INT_FEC_GRA,
        MCF5225_INT_FEC_EBERR,
        MCF5225_INT_FEC_BABT,
        MCF5225_INT_FEC_BABR
    }
};

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MCF5XXX_FEC_get_base_address  
* Returned Value   : Pointer to desired enem device or NULL if not present
* Comments         :
*    This function returns pointer to base address of address space of the 
*    desired enet device. Returns NULL otherwise.
*
*END*----------------------------------------------------------------------*/

pointer MCF5XXX_FEC_get_base_address( uint_32   device )
{ 
    if (device >= MCF5XXX_FEC_DEVICE_COUNT) return NULL;
    return (pointer)(&((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->FEC);
}


 
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MCF5XXX_FEC_io_init
* Returned Value   : none
* Comments         :
*    This function performs BSP-specific initialization related to ENET
*
*END*----------------------------------------------------------------------*/

void MCF5XXX_FEC_io_init( uint_32   device )
{

   if (device >= MCF5XXX_FEC_DEVICE_COUNT) 
      return;

   _bsp_enet_io_init(device);
}
