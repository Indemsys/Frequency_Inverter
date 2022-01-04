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
* $FileName: flexcan_mcf5225.c$
* $Version : 3.6.7.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   Processor family specific file needed for flexcan module.
*   Revision History:
*   Date             Version  Changes
*   ---------        -------  -------
*   Sep 26/08        2.50     Initial version
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <flexcan.h>

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
    if (dev_num != 0)
    {
        return NULL;
    }
   
    return (pointer)(&((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->FCAN);
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
PSP_INTERRUPT_TABLE_INDEX _bsp_get_flexcan_vector 
(
   /* [IN] FlexCAN device number */
    uint_8 dev_num,
   /* [IN] FlexCAN interrupt vector type */
    uint_8 vector_type,
   /* [IN] FlexCAN interrupt vector index */
    uint_32 vector_index
)
{ /* Body */
   volatile PSP_INTERRUPT_TABLE_INDEX index = (PSP_INTERRUPT_TABLE_INDEX) 0;
   
   if (dev_num != 0) 
      return index;
   
   switch (vector_type)
   {
      case FLEXCAN_INT_BUF:
           switch (vector_index)
           {
               case (0):
                  index = MCF5225_INT_FLEXCAN_BUF0;
                  break;
               case (1):
                  index = MCF5225_INT_FLEXCAN_BUF1;
                  break;
               case (2):
                  index = MCF5225_INT_FLEXCAN_BUF2;
                  break;
               case (3):
                  index = MCF5225_INT_FLEXCAN_BUF3;
                  break;
               case (4):
                  index = MCF5225_INT_FLEXCAN_BUF4;
                  break;
               case (5):
                  index = MCF5225_INT_FLEXCAN_BUF5;
                  break;
               case (6):
                  index = MCF5225_INT_FLEXCAN_BUF6;
                  break;
               case (7):
                  index = MCF5225_INT_FLEXCAN_BUF7;
                  break;
               case (8):
                  index = MCF5225_INT_FLEXCAN_BUF8;
                  break;
               case (9):
                  index = MCF5225_INT_FLEXCAN_BUF9;
                  break;
               case (10):
                  index = MCF5225_INT_FLEXCAN_BUF10;
                  break;
               case (11):
                  index = MCF5225_INT_FLEXCAN_BUF11;
                  break;
               case (12):
                  index = MCF5225_INT_FLEXCAN_BUF12;
                  break;
               case (13):
                  index = MCF5225_INT_FLEXCAN_BUF13;
                  break;
               case (14):
                  index = MCF5225_INT_FLEXCAN_BUF14;
                  break;
               case (15):
                  index = MCF5225_INT_FLEXCAN_BUF15;
                  break;
               default:
                  break;
           }
           break;
      case FLEXCAN_INT_ERR:
           index = MCF5225_INT_FLEXCAN_ERR_INT;
           break;
      case FLEXCAN_INT_BOFF:
           index = MCF5225_INT_FLEXCAN_BOFF_INT;
           break;
      default:
           break;
   }
   return index;
} /* EndBody */
