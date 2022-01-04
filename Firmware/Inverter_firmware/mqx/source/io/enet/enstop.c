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
* $FileName: enstop.c$
* $Version : 3.8.5.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   This file contains the MCF52xx Ethernet shutdown
*   functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_shutdown
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Stops the chip.
*
*END*-----------------------------------------------------------------*/
uint_32 ENET_shutdown(_enet_handle   handle /* [IN] the Ethernet state structure */)
{  
    ENET_CONTEXT_STRUCT_PTR    enet_ptr;
    uint_32                    result;
   
    if(handle == NULL)
        return ENETERR_INVALID_DEVICE;
    
    enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;

    /* Make sure upper layers have closed the device.*/
    if (enet_ptr->ECB_HEAD) 
        return ENETERR_DEVICE_IN_USE;
      
    /* Notify the lower layer.*/
    result = (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->STOP)(enet_ptr);
   
    if (result == ENET_OK)
    {
        _lwsem_destroy(&enet_ptr->CONTEXT_LOCK);
        _mqx_unlink_io_component_handle(IO_ENET_COMPONENT,enet_ptr, (pointer *)&enet_ptr->NEXT);
        _mem_free((pointer) enet_ptr);
    }

    return result;
} 


