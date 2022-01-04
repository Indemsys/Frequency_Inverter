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
* $FileName: enaddr.c$
* $Version : 3.0.6.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   This file contains the ENET_get_address utility
*   function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_get_address
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Retrieves the Ethernet address of an initialized device.
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_get_address
   (
      /* [IN] the Ethernet state structure */
      _enet_handle   handle,

      /* [OUT] the local Ethernet address */
      _enet_address  address
   )
{  
   ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;

   eaddrcpy(address, enet_ptr->ADDRESS);

   return ENET_OK;

} 

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_get_mac_address
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Retrieves the Ethernet address of a device.
*
*
*
*END*-----------------------------------------------------------------*/

uint_32 ENET_get_mac_address
   (
      uint_32        device,
      uint_32        value,
      _enet_address  address
   )

{
   return _bsp_get_mac_address(device,value,address);
} 


/* EOF */
