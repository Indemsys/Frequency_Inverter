/**HEADER********************************************************************
* 
* Copyright (c) 2008-2010  Freescale Semiconductor;
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
* $FileName: flexcan_mk60.c$
* $Version : 3.6.3.0$
* $Date    : Nov-3-2010$
*
* Comments:
*
*   Processor family specific file needed for flexcan module.
*   Revision History:
*   Date             Version  Changes
*   ---------        -------  -------
*   Sep 26/10        3.60     Initial version
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_flexcan_base_address  
* Returned Value   : Pointer to desired CAN device or NULL if not present
* Comments         :
*    This function returns pointer to base address of address space of the 
*    desired CAN device. Returns NULL otherwise.
*
*END*********************************************************************/
pointer _bsp_get_flexcan_base_address
(
   /* [IN] FlexCAN device number */
   uint_8 dev_num
)
{ /* Body */
   pointer addr;
    
   switch(dev_num) {
   case 0:
      addr = (pointer)CAN0_BASE_PTR;
      break;
   case 1:
      addr = (pointer)CAN1_BASE_PTR;
      break;
   default:
      addr = NULL;
   }
    
   return addr;
} /* EndBody */

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_flexcan_vector
* Returned Value   : MQX vector number for specified interrupt
* Comments         :
*    This function returns index into MQX interrupt vector table for
*    specified flexcan interrupt. If not known, returns 0.
*
*END*********************************************************************/
IRQInterruptIndex _bsp_get_flexcan_vector 
(
   /* [IN] FlexCAN device number */
    uint_8 dev_num,
   /* [IN] FlexCAN interrupt vector type */
    uint_8 vector_type,
   /* [IN] FlexCAN interrupt vector index */
    uint_32 vector_index
)
{ /* Body */
   IRQInterruptIndex index = (IRQInterruptIndex)0;
   
   switch (dev_num)
   {
      case 0: 
         switch (vector_type)
         {
            case FLEXCAN_INT_BUF:
                 index = INT_CAN0_ORed_Message_buffer;
                 break;
            case FLEXCAN_INT_ERR:
                 index = INT_CAN0_Error;
                 break;
            case FLEXCAN_INT_BOFF:
                 index = INT_CAN0_Bus_Off;
                 break;
            default:
                 break;
         }
         break;
      case 1:    
         switch (vector_type)
         {
            case FLEXCAN_INT_BUF:
                 index = INT_CAN1_ORed_Message_buffer;
                 break;
            case FLEXCAN_INT_ERR:
                 index = INT_CAN1_Error;
                 break;
            case FLEXCAN_INT_BOFF:
                 index = INT_CAN1_Bus_Off;
                 break;
            default:
                 break;
         }
         break;
      default: break;
   }
   
   return index;
} /* EndBody */
